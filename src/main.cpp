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
	fr::ObjRep x (L"x", sf::Color::White, sf::Color::Red, 1.5);
	sf::RenderWindow win(sf::VideoMode(1280, 720), "EXPECT");
	fr::Frame frame(win, font, 32, {0.f, 0.f}, {200.f, 400.f});
	frame.set_char(wall, 0, 0);
	frame.set_char(wall, 1, 0);
	frame.set_char(x, 1, 1);
	frame.set_char(wall, 2, 2);
	
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
		}
		catch (int e)
		{
			std::cout << "ERROR!" << std::endl;
		}
		win.display();
		//frame.set_standard_scale(frame.get_standard_scale()+0.0004);
	}
}
