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

struct Vec2
{
	int x;
	int y;
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
	/* Construct a wall */
	for (int i = 2; i<8; i++)
	{
		entts.push_back(dlr.deal_wall(i, 2));
	}
	
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
			Vec2 dir = {0, 0};
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::L))
				dir.x = 1;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::H))
				dir.x = -1;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::J))
				dir.y = 1;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::K))
				dir.y = -1;
			/* Act out input */
			for (ecs::entity_id ent : ecs::AggView<fa::Position, fa::Drawable,
					fa::Playable>(agg))
			{
				fa::Position* plr_pos = agg.get_cmp<fa::Position>(ent);
				Vec2 upd = {plr_pos->get_x()+dir.x, plr_pos->get_y()+dir.y};
				if (get_at_pos(upd.x, upd.y, entts, &agg).size() == 0)
				{
					plr_pos->set_x(upd.x);
					plr_pos->set_y(upd.y);
					gv.set_char(fr::EMPTY, plr_pos->get_x()-dir.x,
							plr_pos->get_y()-dir.y);
				}
				/* Half-assed safeguards (temporary as everything ofc) */
				if (plr_pos->get_x() >= gv.get_grid_size().x)
					plr_pos->set_x(0);
				if (plr_pos->get_x() < 0)
					plr_pos->set_x(gv.get_grid_size().x-1);
				if (plr_pos->get_y() >= gv.get_grid_size().y)
					plr_pos->set_y(0);
				if (plr_pos->get_y() < 0)
					plr_pos->set_y(gv.get_grid_size().y-1);
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
