#include <iostream>
#include <chrono>
#include "const.h"
#include "frama.hpp"
#include "settings.hpp"
#include "ecs.hpp"
#include "fabric.hpp"

struct Vec2
{
	int x;
	int y;
};

void update_sizes(fr::Frame* gv, fr::Frame* sb, sf::Vector2u size)
{
	SettingContainer s;
	int gv_w = size.x-s.get_sidebar_width(); /* gameview width */
	if (gv_w < 0)
		gv_w = 0;
	 
	try
	{
		gv->set_origin(sf::Vector2i(0,0));
		gv->set_end(sf::Vector2i(gv_w, size.y));
		sb->set_origin(sf::Vector2i(gv_w, 0));
		sb->set_end(sf::Vector2i(size.x, size.y));
	}
	catch(int e)
	{
		if (e == ERR)
		{
			/* Coming soon: A logger */
			std::cout << "Origin smaller than end" << std::endl;
		}
		
	}
}

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

/* Pull in Objects */
void render_gv(int ctr_x, int ctr_y, fr::Frame* gv, ecs::Aggregate *agg)
{
	gv->clear();
	sf::Vector2i gvsize = gv->get_grid_size();
	/* This seems to be the most important performance bottleneck;
	 * Chunks would make sense
	 * TODO: What gets rendered first? So many questions, so little
	 * answers...
	 */
	for (ecs::entity_id ent : ecs::AggView<fa::Position, fa::Drawable>(*agg))
	{
		fa::Position* pos = agg->get_cmp<fa::Position>(ent);
		int x = pos->get_x() - ctr_x + gvsize.x/2, y = pos->get_y() - ctr_y + gvsize.y/2;
		if (x >= 0 && x < gvsize.x && y >= 0 && y < gvsize.y)
		{
			fa::Drawable* rep = agg->get_cmp<fa::Drawable>(ent);
			gv->set_char(fr::ObjRep(rep->ch), x, y);
		}
	}
}

std::vector<ecs::entity_id> entts;


 
/* Start of the main game loop */
int main()
{
	SettingContainer set;
	sf::RenderWindow win(sf::VideoMode(1920, 1080), "expect");
	win.setFramerateLimit(30);
	sf::Font font;
	font.loadFromFile("font.otb");
	/* Game Viewport */
	fr::Frame gv(win, font, 32, sf::Vector2i(0, 0), sf::Vector2i(5, 5));
	fr::Frame sb(win, font, 32, sf::Vector2i(0, 0), sf::Vector2i(5, 5));
	update_sizes(&gv, &sb, win.getSize());
	gv.set_frame_bg(sf::Color::Black);
	sb.set_frame_bg(sf::Color::Green);
	sb.set_standard_scale(0.5f);
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
	while (win.isOpen())
	{
		using namespace std::chrono;
		high_resolution_clock::time_point begin = high_resolution_clock::now();
		bool turn_made = false;
		if (iter == 0)
			turn_made = true;
		sf::Event e;
		while (win.pollEvent(e))
		{
			if (e.type == sf::Event::Closed)
				win.close();
			if (e.type == sf::Event::Resized)
			{
				win.setView(sf::View(sf::FloatRect(0,0,e.size.width, e.size.height)));
				update_sizes(&gv, &sb, win.getSize());
				 
				fa::Position* plr_pos = nullptr;
				/* Accept the first playable object you find as the center of view */
				for (ecs::entity_id ent : ecs::AggView<fa::Position, fa::Drawable,
						fa::Playable>(agg))
					plr_pos = agg.get_cmp<fa::Position>(ent); 
				 
				render_gv(plr_pos->get_x(), plr_pos->get_y(), &gv, &agg);
			}
			/* Input handling */
			if (win.hasFocus() && e.type == sf::Event::KeyPressed)
			{
				Vec2 dir = {0, 0};
				if (e.key.code == sf::Keyboard::Key::L)
					dir.x = 1;
				if (e.key.code == sf::Keyboard::Key::H)
					dir.x = -1;
				if (e.key.code == sf::Keyboard::Key::J)
					dir.y = 1;
				if (e.key.code == sf::Keyboard::Key::K)
					dir.y = -1;
				if (e.key.code == sf::Keyboard::Key::Y)
				{
					dir.y = -1;
					dir.x = -1;
				}
				if (e.key.code == sf::Keyboard::Key::U)
				{
					dir.y = -1;
					dir.x = 1;
				}
				if (e.key.code == sf::Keyboard::Key::N)
				{
					dir.y = 1;
					dir.x = 1;
				}
				if (e.key.code == sf::Keyboard::Key::B)
				{
					dir.y = 1;
					dir.x = -1;
				}
				if (e.key.code == sf::Keyboard::Key::Period)
				{
					turn_made = true;
				}
				if (e.key.code == sf::Keyboard::Key::Add)
				{
					gv.set_standard_scale(gv.get_standard_scale()+set.get_zoom_step());
					turn_made = true;
					update_sizes(&gv, &sb, win.getSize());
				}
				if (e.key.code == sf::Keyboard::Key::Subtract)
				{
					gv.set_standard_scale(gv.get_standard_scale()-set.get_zoom_step());
					turn_made = true;
					update_sizes(&gv, &sb, win.getSize());
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
			 
			render_gv(plr_pos->get_x(), plr_pos->get_y(), &gv, &agg);
		}
		/* Draw Frames */
		bool show_exec_time = false;
		if (gv.draw())
		{
			std::cout << "Updated game viewport!" << std::endl;
			show_exec_time = true;
		}
		if(sb.draw())
		{
			std::cout << "Updated Sidebar viewport!" << std::endl;
			show_exec_time = true;
		}
		high_resolution_clock::time_point end = high_resolution_clock::now();
		if (show_exec_time)
			std::cout << duration_cast<duration<double>>(end - begin).count() << std::endl;
		win.display();
		iter++;
	}
	return 0;
}
