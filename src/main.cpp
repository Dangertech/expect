#include <iostream>
#include <chrono>
#include <unistd.h>
#include "const.h"
#include "settings.hpp"
#include "ecs.hpp"
#include "fabric.hpp"
#include "infra.hpp"
#include "util.hpp"
#include "cli.hpp"


/* NOTE: Having to do THIS for thousands of entities
 * would be inefficient as fuck.
 * Solution: Chonky Boi Chunks???
 */
std::vector<ecs::entity_id> get_at_pos(int x, int y, 
		std::vector<ecs::entity_id> entts, ecs::Aggregate* agg)
{
	std::vector<ecs::entity_id> ret;
	for (int i = 0; i<entts.size(); i++)
	{
		fa::Position* pos = agg->get_cmp<fa::Position>(entts[i]);
		if (pos == nullptr)
			continue;
		if (pos->get_x() == x && pos->get_y() == y)
			ret.push_back(entts[i]); 
	}
	return ret;
}

enum action { NONE, TURN, PICKUP, ZOOM_IN, ZOOM_OUT, ENTER_CLI, EXIT_CLI, ELSE };
struct InputContainer
{
	action act;
	Vec2 dir;
	/* Text Entered in cli mode */
	wchar_t txt;
	static bool incli;
	void print(std::ostream& os)
	{
		std::string act_name;
		switch (act)
		{
			case NONE: act_name = "NONE"; break;
			case TURN: act_name = "TURN"; break;
			case PICKUP: act_name = "PICKUP"; break;
			case ZOOM_IN: act_name = "ZOOM_IN"; break;
			case ZOOM_OUT: act_name = "ZOOM_OUT:"; break;
			case ENTER_CLI: act_name = "ENTER_CLI:"; break;
			case EXIT_CLI: act_name = "EXIT_CLI:"; break;
			case ELSE: act_name = "ELSE"; break;
		}
		os << "Action: " << act_name 
			<< ", Incli: " << incli << ", TXT: " << char(txt)
			<< ", Direction: x=" << dir.x << " y=" << dir.y << std::endl;
	}
};
bool InputContainer::incli = false;

std::vector<InputContainer> process_input(std::vector<sf::Event>& ev)
{
	std::vector<InputContainer> ret;
	bool skiptxt = false;
	for (int i= 0; i<ev.size(); i++)
	{
		/* Local variables that get changed in this
		 * iteration
		 */
		action act = NONE;
		Vec2 dir = {0, 0};
		wchar_t txt = 0;
		bool inc = InputContainer::incli;
		if (ev[i].type == sf::Event::TextEntered && inc && !skiptxt)
		{
			txt = ev[i].text.unicode;
		}
		if (ev[i].type == sf::Event::KeyPressed)
		{
			auto cd = ev[i].key.code;
			if (cd == sf::Keyboard::Key::A)
			{
				if (!inc)
				{
					act = ENTER_CLI; 
					inc = true;
					skiptxt = true;
				}
			}
			else if (cd == sf::Keyboard::Key::Escape)
			{
				if (inc)
				{
					act = EXIT_CLI;
					inc = false;
				}
			}
			else if (cd == sf::Keyboard::Key::L)
				dir.x = 1;
			else if (cd == sf::Keyboard::Key::H)
				dir.x = -1;
			else if (cd == sf::Keyboard::Key::J)
				dir.y = 1;
			else if (cd == sf::Keyboard::Key::K)
				dir.y = -1;
			else if (cd == sf::Keyboard::Key::Y)
			{
				dir.y = -1;
				dir.x = -1;
			}
			else if (cd == sf::Keyboard::Key::U)
			{
				dir.y = -1;
				dir.x = 1;
			}
			else if (cd == sf::Keyboard::Key::N)
			{
				dir.y = 1;
				dir.x = 1;
			}
			else if (cd == sf::Keyboard::Key::B)
			{
				dir.y = 1;
				dir.x = -1;
			}
			else if (cd == sf::Keyboard::Key::Period)
			{
				act = TURN;
			}
			else if (cd == sf::Keyboard::Key::Comma)
				act = PICKUP;
			else if (cd == sf::Keyboard::Key::Add)
				act = ZOOM_IN;
			else if (cd == sf::Keyboard::Key::Subtract)
				act = ZOOM_OUT;
		}
		 
		/* Push changed locals to actual containers */
		if (dir.x != 0 || dir.y != 0)
			act = TURN;
		InputContainer ipt = {act, dir, txt};
		InputContainer::incli = inc;
		ipt.print(std::cout);
		ret.push_back(ipt);
	}
	return ret;
}

std::vector<ecs::entity_id> entts;

 
/* Start of the main game loop */
int main()
{
	SettingContainer set;
	/* Manages in-game entities through an ECS */
	ecs::Aggregate agg;
	/* Manages the command-line-interface to the right of the screen */
	cli::CliData cli;
	/* Manages everything related to displaying graphics,
	 * without cluttering and confusing behaviours
	 */
	in::GfxManager gfx(agg, cli);
	cli.log(cli::LogEntry(L"Welcome, Truthseeker...", cli::MESSAGE));
	/* Frontend for creating entities through an aggregate */
	fa::EntityDealer dlr(&agg);
	/* Construct a player Object and place its id in the entts vector */
	entts.push_back(dlr.deal_player(0, 0));
	entts.push_back(dlr.deal_item(0, 1));
	/* Construct some walls */
	for (int i = -30; i<30; i++)
	{
		for (int j = -30; j<30; j++)
		{
			entts.push_back(dlr.deal_wall(i*4-4, j*5-5));
		}
	}
	
	int iter = 0;
	while (gfx.win_open())
	{
		/* Get time to process this loop for fps matching and debugging purposes */
		using namespace std::chrono;
		high_resolution_clock::time_point begin = high_resolution_clock::now();
		 
		/* Get Input event loop:
		 * Window events are processed by gfx internally on function call,
		 * but it spews out all events to process events it shouldn't handle
		 * externally (namely HIDs)
		 */
		std::vector<sf::Event> ev = gfx.get_events();
		std::vector<InputContainer> ipt = process_input(ev);
		bool upd_screen = false;
		for (int i = 0; i < ipt.size(); i++)
		{
			if (iter == 0)
				ipt[i].act = ELSE;
			 
			if (InputContainer::incli)
			{
				upd_screen = true;
				cli.set_active(true);
				if (cli.add_to_bfr(ipt[i].txt))
				{
					InputContainer::incli = false;
					cli.set_active(false);
				}
				/* Skip acting out input */
				continue;
			}
			else
			{
				cli.set_active(false);
				upd_screen = true;
			}
			/* Act out input */
			for (ecs::entity_id ent : ecs::AggView<fa::Position, fa::Drawable,
					fa::Playable>(agg))
			{
				fa::Position* plr_pos = agg.get_cmp<fa::Position>(ent);
				switch (ipt[i].act)
				{
					case TURN:
					{
						upd_screen = true;
						Vec2 upd = {plr_pos->get_x()+ipt[i].dir.x, plr_pos->get_y()+ipt[i].dir.y};
						std::vector<ecs::entity_id> blockers;
						for (ecs::entity_id ent : ecs::AggView<fa::Blocking>(agg))
							blockers.push_back(ent);
						if (get_at_pos(upd.x, upd.y, blockers, &agg).size() == 0)
						{
							ipt[i].act = TURN;
							plr_pos->set_x(upd.x);
							plr_pos->set_y(upd.y);
						}
						break;
					}
					case PICKUP:
					{
						upd_screen = true;
						bool items = false;
						for (ecs::entity_id itm : ecs::AggView<fa::Position, 
								fa::Pickable>(agg))
						{
							items = true;
							fa::Position* itm_pos = agg.get_cmp<fa::Position>(itm);
							if (itm_pos->get_x() == plr_pos->get_x()
									&& itm_pos->get_y() == plr_pos->get_y())
							{
								agg.destroy_entity(itm);
								cli.log(cli::LogEntry(L"You picked up an item!", cli::MESSAGE));
							}
							else
								cli.log(cli::LogEntry(L"Nothing to pick up here!", cli::MESSAGE));
						}
						if (!items)
							cli.log(cli::LogEntry(L"Nothing to pick up here!", cli::MESSAGE));
						break;
					}
					case ZOOM_IN:
						upd_screen = true;
						gfx.adjust_zoom(set.get_zoom_step());
						break;
					case ZOOM_OUT:
						upd_screen = true;
						gfx.adjust_zoom(-set.get_zoom_step());
						break;
					default:
						break;
				}
			}
		}
		
		bool updated = false;
		if (upd_screen)
		{
			fa::Position* plr_pos = nullptr;
			/* Accept the first playable object you find as the center of view */
			for (ecs::entity_id ent : ecs::AggView<fa::Position, fa::Drawable,
					fa::Playable>(agg))
			{
				plr_pos = agg.get_cmp<fa::Position>(ent); 
				break;
			}
			 
			gfx.set_cam_center({plr_pos->get_x(), plr_pos->get_y()});
			if (gfx.render_gv(true))
				updated = gfx.display_frames();
		}
		else
		{
			/* Will only actually run if the
			 * GfxManager has a reason to redraw
			 * the gameview by itself (window resized event, etc.);
			 * Otherwise exits immediately
			 */
			if(gfx.render_gv(false))
				updated = gfx.display_frames();
		}
		high_resolution_clock::time_point end = high_resolution_clock::now();
		double time_spent = duration_cast<duration<double>>(end-begin).count();
		if (updated)
		{
			/*
			cli::LogEntry e(L"Time spent processing: " + std::to_wstring(time_spent), cli::DEBUG);
			cli.log(e);
			*/
		}
		/* Wait manually to meet the FPS requirement */
		double goal_time = 1.0/set.get_fps()-time_spent;
		if (goal_time > 0.0)
		{
			/*
			std::cout << "Waiting for " << goal_time << " seconds!" << std::endl;
			*/
			usleep(goal_time*1000000);
		}
		/*
		else
			std::cout << "Hanging behind!" << std::endl;
			*/
		iter++;
	}
	return 0;
}
