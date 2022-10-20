#include <iostream>
#include <chrono>
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
	std::cout << "Started execution." << std::endl;
	SettingContainer set;
	/* World Storage;
	 * Each Chunk is an 
	 */
	//std::unordered_map<unsigned long,ecs::Aggregate> chunks;
	WorldContainer wrld;
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
	/* Construct a player Object and place its id in the entts vector */
	entts.push_back(fa::deal_player(0, 0, 0, agg));
	entts.push_back(fa::deal_item(0, 1, 0, agg));
	entts.push_back(fa::deal_item(1, 1, 0, agg));
	agg.get_cmp<fa::Eatable>(entts[entts.size()-1])->type = fa::Eatable::RATION;
	/* Construct some walls */
	for (int z = 0; z < 5; z++)
	{
		for (int i = -30; i<30; i++)
		{
			for (int j = -30; j<30; j++)
			{
				ecs::entity_id e = fa::deal_wall(i*4-4, j*4-4, z, agg);
				if (j == 5)
				{
					fa::Paintable* p = agg.get_cmp<fa::Paintable>(e);
					if (p)
					{
						p->color = {0,255,0};
						p->visibility = 100.f;
					}
				}
				if (j == 2)
				{
					fa::Flammable* f = agg.get_cmp<fa::Flammable>(e);
					if (f)
					{
						f->burning = true;
					}
				}
				entts.push_back(e);
			}
		}
	}
	for (int z = 1; z<3; z++)
	{
		for (int i = -10; i<10; i++)
		{
			for (int j = -10; j<10; j++)
			{
				entts.push_back(fa::deal_wall(i*4-2, j*4-2, z, agg));
			}
		}
	}
	for (int i = -30*4-4; i<30*4-4; i++)
	{
		for (int j = -30*4-4; j<30*4-4; j++)
		{
			ecs::entity_id e = fa::deal_wall(i, j, -1, agg);
			if (j == 2)
			{
				fa::Flammable* f = agg.get_cmp<fa::Flammable>(e);
				if (f)
				{
					f->burning = true;
				}
			}
			entts.push_back(e);
		}
	}
	
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
		/* Input reaction logic */
		bool skiptxt = false;
		for (int i = 0; i < ev.size(); i++)
		{
			if (cli.get_active() && !skiptxt)
			{
				/* Process as input for cli */
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
						ipt::process_input(final_string, agg, cli, gfx, set, skiptxt);
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
				if (ipt::cmdmap.find(ev[i].key.code) == ipt::cmdmap.end())
				{
					cli.log({L"No command assigned to this key.", cli::DEBUG});
					continue;
				}
				/* Execute command of entered text */
				std::wstring string = ipt::cmdmap.at(ev[i].key.code);
				std::vector<cli::LogEntry> response = 
					ipt::process_input(string, agg, cli, gfx, set, skiptxt);
				/* Make the response graphically available to the user */
				for (int i = 0; i<response.size(); i++)
					cli.log(response[i]);
			}
		}
		
		/* Rendering Execution */
		fa::Position* plr_pos = nullptr;
		/* Accept the first playable object you find as the center of view */
		for (ecs::entity_id ent : ecs::AggView<fa::Position, fa::Playable>(agg))
		{
			plr_pos = agg.get_cmp<fa::Position>(ent); 
			break;
		}
		 
		gfx.set_cam_center({plr_pos->x, plr_pos->y, plr_pos->z});
		 /* Draw all frames */
		gfx.render();
		/* Wait some time to meet the fps requirement and not waste 
		 * computing power for rendering some text at 500 fps
		 */
		high_resolution_clock::time_point end = high_resolution_clock::now();
		gfx.delay(duration_cast<duration<double>>(end-begin).count());
	}
	return 0;
}
