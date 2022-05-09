#include <iostream>
#include "frama.hpp"
#include <SFML/Graphics.hpp>


int main()
{
	sf::Font font;
	font.loadFromFile("font.ttf");
	fr::ObjRep wall
	(L"#", font, sf::Color(255, 180, 45, 255), 
		sf::Color::Green, 28, sf::Color(255, 128, 37, 255),
		2,
		0);
	sf::RenderWindow win(sf::VideoMode(1280, 720), "EXPECT");
	fr::Frame frame(win);
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
