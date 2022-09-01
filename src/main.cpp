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
#include "input.hpp"
#include "commands.hpp"





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
	cli.log(cli::LogEntry(std::wstring(L"This is the BECOME interface, ") + VERSION, cli::MESSAGE));
	cli.log(cli::LogEntry(std::wstring(L"Codename: \"") + VERSION_NAME + L"\"", cli::MESSAGE));
	cli.log(cli::LogEntry(std::wstring(L"Last updated on: ") + VERSION_DATE, cli::MESSAGE));
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
	/* TODO: Find a better place for this variable */
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
		bool upd_screen = false;
		bool skiptxt = false;
		for (int i = 0; i < ev.size(); i++)
		{
			 
			if (cli.get_active() && !skiptxt)
			{
				/* Process as input for cli */
				upd_screen = true;
				cli.set_active(true);
				if (ev[i].type != sf::Event::TextEntered)
				{
					/* Special rules for command to exit CLI */
					if (ev[i].type == sf::Event::KeyPressed)
					{
						if (ipt::cmdmap.find(ev[i].key.code) != ipt::cmdmap.end())
						{
							if (ipt::cmdmap.at(ev[i].key.code) == L"cli exit")
							{
								cmd::cli({L"exit"}, skiptxt, cli);
							}
						}
					}
					continue;
				}
				if (ev[i].text.unicode == 13) /* Enter Key */
				{
					cli.set_active(false);
					std::wstring final_string = cli.push_bfr();
					std::vector<cli::LogEntry> response = 
						ipt::process_input(final_string, agg, entts, cli, gfx, set, skiptxt);
					/* Make the response graphically available to the user */
					for (int i = 0; i<response.size(); i++)
						cli.log(response[i]);
				}
				else
					cli.add_to_bfr(ev[i].text.unicode);
			}
			else
			{
				/* Process as key input that might be mapped to a command */
				if (ev[i].type != sf::Event::KeyPressed)
					continue;
				cli.set_active(false);
				upd_screen = true;
				if (ipt::cmdmap.find(ev[i].key.code) == ipt::cmdmap.end())
				{
					cli.log({L"No command assigned to this key.", cli::DEBUG});
					continue;
				}
				/* Execute command of entered text */
				std::wstring string = ipt::cmdmap.at(ev[i].key.code);
				std::vector<cli::LogEntry> response = 
					ipt::process_input(string, agg, entts, cli, gfx, set, skiptxt);
				/* Make the response graphically available to the user */
				for (int i = 0; i<response.size(); i++)
					cli.log(response[i]);
			}
		}
		
		/* Rendering Execution */
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
