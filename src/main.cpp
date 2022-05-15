#include <iostream>
#include "frama.hpp"
#include <SFML/Graphics.hpp>


int main()
{
	sf::Font font;
	font.loadFromFile("font.otb");
	fr::ObjRep wall
	(L"#", sf::Color(255, 255, 255, 255), 
		sf::Color(0, 255, 0, 128), 1);
	fr::ObjRep excl (L"!");
	sf::RenderWindow win(sf::VideoMode(1280, 720), "EXPECT");
	fr::Frame frame(win, font, 32, sf::Vector2f(0.f, 200.f), 
		sf::Vector2f(200.f, 400.f));
	frame.set_frame_bg(sf::Color(255, 0, 0, 255));
	fr::Frame sidebar(win, font, 32, sf::Vector2f(920.f, 0.f), 
		sf::Vector2f(1280.f, 720.f));
	sidebar.set_frame_bg(sf::Color(0, 0, 0, 255));
	frame.set_standard_scale(1);
	frame.set_fit_to_text(true);
	for (int y = 0; y < 20; y++)
	{
		for (int x = 0; x < 100; x++)
		{
			if (y%2 == 0)
			{
				if (x%2 == 0)
					frame.set_char(wall, x, y);
				else
					frame.set_char(excl, x, y);
			}
			else
			{
				if (x%2 == 1)
					frame.set_char(wall, x, y);
				else
					frame.set_char(excl, x, y);
			}
		}
	}
	
	while (win.isOpen())
	{
		sf::Event event;
		while(win.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				win.close();
		}
		 
		win.clear();
		try
		{
			frame.draw();
			//sidebar.draw();
		}
		catch (int e)
		{
			std::cout << "Grid empty!" << std::endl;
		}
		win.display();
		frame.set_standard_scale(frame.get_standard_scale()+0.0004);
		frame.set_origin(sf::Vector2f(frame.get_origin().x+0.05, 
			frame.get_origin().y));
		frame.set_end(sf::Vector2f(frame.get_end().x+0.05,
			frame.get_end().y));
	}
}
