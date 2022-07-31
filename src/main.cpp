#include <iostream>
#include <chrono>
#include "const.h"
#include "settings.hpp"
#include "ecs.hpp"
#include "fabric.hpp"
#include "infra.hpp"
#include "util.hpp"


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

std::vector<ecs::entity_id> entts;

 
/* Start of the main game loop */
int main()
{
	SettingContainer set;
	/* Manages everything related to displaying graphics,
	 * without cluttering and confusing behaviours
	 */
	ecs::Aggregate agg;
	in::GfxManager gfx(&agg);
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
		using namespace std::chrono;
		high_resolution_clock::time_point begin = high_resolution_clock::now();
		enum action { NONE, TURN, PICKUP, ELSE } act = NONE;
		if (iter == 0)
			act = ELSE;
		std::vector<sf::Event> ev = gfx.get_events();
		/* Input handling */
		for (int i= 0; i<ev.size(); i++)
		{
			if (ev[i].type == sf::Event::KeyPressed)
			{
				Vec2 dir = {0, 0};
				auto cd = ev[i].key.code;
				if (cd == sf::Keyboard::Key::L)
					dir.x = 1;
				if (cd == sf::Keyboard::Key::H)
					dir.x = -1;
				if (cd == sf::Keyboard::Key::J)
					dir.y = 1;
				if (cd == sf::Keyboard::Key::K)
					dir.y = -1;
				if (cd == sf::Keyboard::Key::Y)
				{
					dir.y = -1;
					dir.x = -1;
				}
				if (cd == sf::Keyboard::Key::U)
				{
					dir.y = -1;
					dir.x = 1;
				}
				if (cd == sf::Keyboard::Key::N)
				{
					dir.y = 1;
					dir.x = 1;
				}
				if (cd == sf::Keyboard::Key::B)
				{
					dir.y = 1;
					dir.x = -1;
				}
				if (cd == sf::Keyboard::Key::Period)
				{
					act = TURN;
				}
				if (cd == sf::Keyboard::Key::Comma)
				{
					act = PICKUP;
					/*
					std::vector<ecs::entity_id> here = 
							get_at_pos(plr_pos->get_x(), plr_pos->get_y(),
							entts, &agg);
							*/
				}
				if (cd == sf::Keyboard::Key::Add)
				{
					gfx.adjust_zoom(set.get_zoom_step());
					act = ELSE;
				}
				if (cd == sf::Keyboard::Key::Subtract)
				{
					gfx.adjust_zoom(-set.get_zoom_step());
					act = ELSE;
				}
				 
				if (dir.x != 0 || dir.y != 0)
					act = TURN;
				/* Act out input */
				for (ecs::entity_id ent : ecs::AggView<fa::Position, fa::Drawable,
						fa::Playable>(agg))
				{
					fa::Position* plr_pos = agg.get_cmp<fa::Position>(ent);
					switch (act)
					{
						case TURN:
						{
							Vec2 upd = {plr_pos->get_x()+dir.x, plr_pos->get_y()+dir.y};
							std::vector<ecs::entity_id> blockers;
							for (ecs::entity_id ent : ecs::AggView<fa::Blocking>(agg))
								blockers.push_back(ent);
							if (get_at_pos(upd.x, upd.y, blockers, &agg).size() == 0)
							{
								act = TURN;
								plr_pos->set_x(upd.x);
								plr_pos->set_y(upd.y);
							}
							break;
						}
						case PICKUP:
						{
							for (ecs::entity_id itm : ecs::AggView<fa::Position, 
									fa::Pickable>(agg))
							{
								fa::Position* itm_pos = agg.get_cmp<fa::Position>(itm);
								if (itm_pos->get_x() == plr_pos->get_x()
										&& itm_pos->get_y() == plr_pos->get_y())
									agg.destroy_entity(itm);
							}
							break;
						}
					}
				}
			}
		}
		
		if (act != NONE)
		{
			fa::Position* plr_pos = nullptr;
			/* Accept the first playable object you find as the center of view */
			for (ecs::entity_id ent : ecs::AggView<fa::Position, fa::Drawable,
					fa::Playable>(agg))
				plr_pos = agg.get_cmp<fa::Position>(ent); 
			 
			gfx.set_cam_center({plr_pos->get_x(), plr_pos->get_y()});
			gfx.render_gv(true);
		}
		else
			/* Will only actually run if the
			 * GfxManager has a reason to redraw
			 * the gameview by itself (window resized event, etc.);
			 * Otherwise exits immediately
			 */
			gfx.render_gv(false);
		/* Draw Frames */
		bool updated = gfx.display_frames();
		high_resolution_clock::time_point end = high_resolution_clock::now();
		if (updated)
			std::cout << duration_cast<duration<double>>(end - begin).count() << std::endl;
		iter++;
	}
	return 0;
}
