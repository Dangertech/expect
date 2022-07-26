#include <iostream>
#include "const.h"
#include "frama.hpp"
#include "settings.hpp"
#include "ecs.hpp"
#include "fabric.hpp"

void update_sizes(fr::Frame &gv, fr::Frame &sb, sf::Vector2u size)
{
	SettingContainer s;
	int gv_w = size.x-s.get_sidebar_width(); /* gameview width */
	if (gv_w < 0)
		gv_w = 0;
	 
	try
	{
		gv.set_origin(sf::Vector2i(0,0));
		gv.set_end(sf::Vector2i(gv_w, size.y));
		sb.set_origin(sf::Vector2i(gv_w, 0));
		sb.set_end(sf::Vector2i(size.x, size.y));
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

std::vector<ecs::entity_id> entts;

struct Position
{
	int x; int y;
};
 
/* Start of the main game loop */
int main()
{
	sf::RenderWindow win(sf::VideoMode(1920, 1080), "expect");
	win.setFramerateLimit(30);
	sf::Font font;
	font.loadFromFile("font.otb");
	/* Game Viewport */
	fr::Frame gv(win, font, 32, sf::Vector2i(0, 0), sf::Vector2i(5, 5));
	fr::Frame sb(win, font, 32, sf::Vector2i(0, 0), sf::Vector2i(5, 5));
	update_sizes(gv, sb, win.getSize());
	gv.set_frame_bg(sf::Color::Red);
	sb.set_frame_bg(sf::Color::Green);
	sb.set_standard_scale(0.5f);
	ecs::Aggregate agg;
	fa::EntityDealer dlr(agg);
	/* Construct a player Object and place its id in the entts vector */
	entts.push_back(dlr.deal_player(3, 3));
	entts.push_back(dlr.deal_player(5, 8));
	
	int iter = 0;
	while (win.isOpen())
	{
		sf::Event e;
		while (win.pollEvent(e))
		{
			if (e.type == sf::Event::Closed)
				win.close();
			if (e.type == sf::Event::Resized)
			{
				win.setView(sf::View(sf::FloatRect(0,0,e.size.width, e.size.height)));
				update_sizes(gv, sb, win.getSize());
			}
		}
		/* Input handling */
		if (win.hasFocus())
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
			{
				for (ecs::entity_id ent : ecs::AggView<fa::Position, fa::Drawable,
						fa::Playable>(agg))
				{
					fa::Position* plr_pos = agg.get_cmp<fa::Position>(ent);
					plr_pos->set_x(plr_pos->get_x()+1);
					gv.set_char(fr::EMPTY, plr_pos->get_x()-1,
							plr_pos->get_y());
					if (plr_pos->get_x() >= gv.get_grid_size().x)
						plr_pos->set_x(0);
				}
			}
		}
		/* Pull in Objects */
		for (ecs::entity_id ent : ecs::AggView<fa::Position>(agg))
		{
			fa::Position* pos = agg.get_cmp<fa::Position>(ent);
			fa::Drawable* rep = agg.get_cmp<fa::Drawable>(ent);
			gv.set_char(fr::ObjRep(rep->ch), pos->get_x(), pos->get_y());
		}
		/* Draw Frames */
		/* Why the FUCK is this required? Does
		 * the issue lie in win.display()?
		 */
		if (iter <= 1)
		{
			gv.force_update();
			sb.force_update();
		}
		if (gv.draw())
			std::cout << "Updated game viewport!" << std::endl;
		if(sb.draw())
			std::cout << "Updated Sidebar viewport!" << std::endl;
		win.display();
		iter++;
	}
	return 0;
}
