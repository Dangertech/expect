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
	frame.set_char(wall, 0, 0);
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
		frame.draw();
		win.display();
	}
}
