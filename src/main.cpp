#include <iostream>
#include "frama.hpp"
#include <SFML/Graphics.hpp>


int main()
{
	sf::Font font;
	font.loadFromFile("font.otb");
	fr::ObjRep wall
	(L"#", sf::Color(255, 255, 255, 255), 
		sf::Color::Green, 1);
	sf::RenderWindow win(sf::VideoMode(1280, 720), "EXPECT");
	fr::Frame frame(win, font, 32, {0.f, 200.f}, {200.f, 400.f});
	frame.set_frame_bg(sf::Color(255, 0, 0, 255));
	fr::Frame sidebar(win, font, 32, {920.f, 0.f}, {1280.f, 720.f});
	sidebar.set_frame_bg(sf::Color(0, 0, 0, 255));
	frame.set_standard_scale(1);
	for (int y = 0; y < 20; y++)
	{
		for (int x = 0; x < 100; x++)
		{
			if (y%2 == 0)
			{
				if (x%2 == 0)
					frame.set_char(wall, x, y);
			}
			else
			{
				if (x%2 == 1)
					frame.set_char(wall, x, y);
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
		//frame.set_standard_scale(frame.get_standard_scale()+0.0004);
		frame.set_origin({frame.get_origin().first+0.05, frame.get_origin().second});
		frame.set_end({frame.get_end().first+0.05, frame.get_end().second});
	}
}
