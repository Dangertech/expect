#include <iostream>
#include "const.h"
#include "frama.hpp"
#include "settings.hpp"
#include "ecs.hpp"

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
	/* Pretty useless, I know;
	 * Has to be refined to actually make
	 * sense
	 */
	public:
		int get_x() { return x; }
		void set_x(int my_x) { x = my_x; }
		int get_y() { return y; }
		void set_y(int my_y) { y = my_y;}
	private:
		int x = 5, y = 5;
};
 
/* Start of the main game loop */
int main()
{
	sf::RenderWindow win(sf::VideoMode(1920, 1080), "expect");
	win.setVerticalSyncEnabled(true);
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
	entts.push_back(agg.new_entity());
	agg.add_cmp<Position>(entts[0]);
	
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
				Position* test_pos = agg.get_cmp<Position>(entts[0]);
				test_pos->set_x(test_pos->get_x()+1);
				gv.set_char(fr::EMPTY, test_pos->get_x()-1,
						test_pos->get_y());
				if (test_pos->get_x() >= gv.get_grid_size().x)
					test_pos->set_x(0);
			}
		}
		/* Pull in Objects */
		for (ecs::entity_id ent : ecs::AggView<Position>(agg))
		{
			Position* pos = agg.get_cmp<Position>(ent);
			gv.set_char(fr::ObjRep(L"W"), pos->get_x(), pos->get_y());
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
