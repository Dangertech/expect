#include <iostream>
#include "const.h"
#include "frama.hpp"
#include "settings.hpp"

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
		win.clear();
		gv.draw();
		sb.draw();
		win.display();
	}
	return 0;
}
