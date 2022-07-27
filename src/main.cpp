#include <iostream>
#include <chrono>
#include "const.h"
#include "settings.hpp"
#include "ecs.hpp"
#include "fabric.hpp"
#include "infra.hpp"

struct Vec2
{
	int x;
	int y;
};

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
	in::GfxManager gfx;
	ecs::Aggregate agg;
	fa::EntityDealer dlr(agg);
	/* Construct a player Object and place its id in the entts vector */
	entts.push_back(dlr.deal_player(0, 0));
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
		bool turn_made = false;
		if (iter == 0)
			turn_made = true;
		std::vector<sf::Event> ev = gfx.get_events();
		/* Input handling */
		for (sf::Event e : ev)
		{
			if (e.type == sf::Event::KeyPressed)
			{
				Vec2 dir = {0, 0};
				auto cd = e.key.code;
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
					turn_made = true;
				}
				if (cd == sf::Keyboard::Key::Add)
				{
					gfx.adjust_zoom(set.get_zoom_step());
					turn_made = true;
				}
				if (cd == sf::Keyboard::Key::Subtract)
				{
					gfx.adjust_zoom(-set.get_zoom_step());
					turn_made = true;
				}
				/* Act out input */
				for (ecs::entity_id ent : ecs::AggView<fa::Position, fa::Drawable,
						fa::Playable>(agg))
				{
					fa::Position* plr_pos = agg.get_cmp<fa::Position>(ent);
					Vec2 upd = {plr_pos->get_x()+dir.x, plr_pos->get_y()+dir.y};
					if (get_at_pos(upd.x, upd.y, entts, &agg).size() == 0)
					{
						turn_made = true;
						plr_pos->set_x(upd.x);
						plr_pos->set_y(upd.y);
					}
				}
			}
		}
		
		if (turn_made)
		{
			fa::Position* plr_pos = nullptr;
			/* Accept the first playable object you find as the center of view */
			for (ecs::entity_id ent : ecs::AggView<fa::Position, fa::Drawable,
					fa::Playable>(agg))
				plr_pos = agg.get_cmp<fa::Position>(ent); 
			 
			gfx.render_gv(plr_pos->get_x(), plr_pos->get_y(), &agg);
		}
		/* Draw Frames */
		bool updated = gfx.display_frames();
		high_resolution_clock::time_point end = high_resolution_clock::now();
		if (updated)
			std::cout << duration_cast<duration<double>>(end - begin).count() << std::endl;
		iter++;
	}
	return 0;
}
