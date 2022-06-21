#include <iostream>
#include "frama.hpp"
#include "const.h"

fr::ObjRep wall
(L"#", sf::Color(128, 128, 255, 255), 
	sf::Color(0, 255, 0, 128), 1);
fr::ObjRep excl (L"!");
fr::ObjRep mark(L"X", sf::Color(239, 108, 29, 255));
fr::ObjRep empty(L" ");

std::vector<sf::CircleShape> pos_circles(fr::Frame frame)
{
	sf::Vector2<int> grid_size = frame.get_grid_size();
	std::vector<sf::CircleShape> circle_queue;
	for (int x = 0; x<grid_size.x; x++)
	{
		for (int y = 0; y<grid_size.y; y++)
		{
			sf::Vector2f pos;
			try
			{
				pos = frame.get_char_pos(x, y, fr::CharPoint::CENTER);
			}
			catch(int e)
			{
				std::cout << "Position of " << x << ", " << y << " is out of bounds\n";
				pos.x = 20;
				pos.y = 20;
			}
			 
			int radius = 4;
			sf::CircleShape circ(radius);
			circ.setPosition(pos.x-radius, pos.y-radius);
			circle_queue.push_back(circ);
		}
	}
	return circle_queue;
}

void big_move_and_size(sf::Font &font, sf::RenderWindow &win)
{
	fr::Frame frame(win, font, 32, sf::Vector2f(0.f, 200.f), 
		sf::Vector2f(200.f, 400.f));
	frame.set_frame_bg(sf::Color(255, 0, 0, 255));
	frame.set_standard_scale(1);
	frame.set_fit_to_text(true);
	sf::Vector2<int> grid_size = frame.get_grid_size();
	for (int y = 0; y < grid_size.y; y++)
	{
		for (int x = 0; x < grid_size.x; x++)
		{
			if (y%2 == 0)
			{
				if (x%2 == 0)
				{
					try{ frame.set_char(wall, x, y); }
					catch(int e){}
				}
				/*
				else
					frame.set_char(excl, x, y);
					*/
			}
			else
			{
				if (x%2 == 1)
					try{ frame.set_char(wall, x, y); }
					catch(int e){}
				else
					try{ frame.set_char(wall, x, y); }
					catch(int e){}
			}
		}
	}
	
	while (win.isOpen())
	{

		auto circle_queue = pos_circles(frame);
		sf::Event event;
		
		while(win.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				win.close();
			else if (event.type == sf::Event::MouseMoved)
			{
				sf::Vector2<int> mp = sf::Mouse::getPosition(win);
				try
				{
					sf::Vector2<int> grid_mp = frame.get_char_at(mp.x, mp.y);
					std::cout << grid_mp.x << " " << grid_mp.y << std::endl;
					frame.set_char(mark, grid_mp.x, grid_mp.y);
				}
				catch(int e)
				{
					std::cout << "Mouse not over grid" << std::endl;
				}
			}
		}
		/* fit to text press switch */
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
			frame.set_fit_to_text(false);
		else
			frame.set_fit_to_text(true);
		/* Reset button */
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
		{
			frame.set_standard_scale(1);
			frame.set_origin(sf::Vector2f(0.f, 200.f));
			frame.set_end(sf::Vector2f(200.f, 400.f));
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
		for (auto x : circle_queue)
		{
			win.draw(x);
		}
		win.display();
		std::cout << "Current size of any character: "
			<< frame.get_char_size().x << " " << frame.get_char_size().y << "\n";
		sf::Vector2f std = frame.get_standard_char_size();
		std::cout << "Size of an unscaled character: " << std.x << " " << std.y << std::endl;
		frame.set_standard_scale(frame.get_standard_scale()+0.0002);
		frame.set_origin(sf::Vector2f(frame.get_origin().x+0.01, 
			frame.get_origin().y));
		frame.set_end(sf::Vector2f(frame.get_end().x+0.001,
			frame.get_end().y));
	}
}

void irregular_positions(sf::Font &font, sf::RenderWindow &win)
{
	fr::Frame frame(win, font, 32, sf::Vector2f(0.f, 200.f), 
		sf::Vector2f(200.f, 400.f));
	frame.set_frame_bg(sf::Color(255, 0, 0, 255));
	frame.set_char(wall, 0, 0);
	frame.set_char(wall, 0, 1);
	frame.set_char(wall, 2, 4);
	frame.set_char(wall, 5, 2);
	 
	/* Test get_grid_size() and get_char_pos() */
	auto circle_queue = pos_circles(frame);
	while (win.isOpen())
	{
		/* I know that this is super shitty and doesn't
		 * really work but it shows that the function works 
		 */
		auto grid_size = frame.get_grid_size();
		for (int i = 0; i<grid_size.x; i++)
		{
			for (int j = 0; j<grid_size.y; j++)
			{
				if (frame.get_char(i, j) == mark)
					frame.set_char(empty, i, j);
			}
		}
		sf::Event event;
		while(win.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				win.close();
			else if (event.type == sf::Event::MouseMoved)
			{
				sf::Vector2<int> mp = sf::Mouse::getPosition(win);
				try
				{
					sf::Vector2<int> grid_mp = frame.get_char_at(mp.x, mp.y);
					std::cout << grid_mp.x << " " << grid_mp.y << std::endl;
					frame.set_char(mark, grid_mp.x, grid_mp.y);
				}
				catch(int e)
				{
					std::cout << "Mouse not over grid" << std::endl;
				}
			}
		}
		 
		win.clear();
		try
		{
			frame.draw();
		}
		catch (int e)
		{
			std::cout << "Grid empty!" << std::endl;
		}
		for (auto x : circle_queue)
		{
			win.draw(x);
		}
		win.display();
	}
}

void move(sf::Font &font, sf::RenderWindow &win)
{
	fr::Frame frame(win, font, 32, sf::Vector2f(0.f, 200.f), 
		sf::Vector2f(200.f, 400.f));
	frame.set_frame_bg(sf::Color(255, 0, 0, 255));
	frame.set_standard_scale(1);
	frame.set_fit_to_text(true);
	sf::Vector2<int> grid_size = frame.get_grid_size();
	for (int y = 0; y < grid_size.y; y++)
	{
		for (int x = 0; x < grid_size.x; x++)
		{
			if (y%2 == 0)
			{
				if (x%2 == 0)
					frame.set_char(wall, x, y);
				/*
				else
					frame.set_char(excl, x, y);
					*/
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
		auto circle_queue = pos_circles(frame);
		sf::Event event;
		/* fit to text press switch */
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
			frame.set_fit_to_text(false);
		else
			frame.set_fit_to_text(true);
		/* Reset button */
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
		{
			frame.set_standard_scale(1);
			frame.set_origin(sf::Vector2f(0.f, 200.f));
			frame.set_end(sf::Vector2f(200.f, 400.f));
		}
		 
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
		for (auto x : circle_queue)
		{
			win.draw(x);
		}
		win.display();
		frame.set_origin(sf::Vector2f(frame.get_origin().x+0.1,
			frame.get_origin().y));
		frame.set_end(sf::Vector2f(frame.get_end().x+0.1,
			frame.get_end().y));
	}
}

/* Constructs a frame that should contain *exactly*
 * a 5x5 grid
 */
void even(sf::Font &font, sf::RenderWindow &win)
{
	fr::Frame frame(win, font, 32, sf::Vector2f(0.f, 0.f), 
		sf::Vector2f(80.f, 160.f));
	frame.set_frame_bg(sf::Color(255, 0, 0, 255));
	frame.set_standard_scale(1);
	while (win.isOpen())
	{
		for (int i = 0; i< 20; i++)
		{
			for (int j = 0; j<20; j++)
			{
				try
				{
					frame.set_char(wall, i, j);
				}
				catch(int e)
				{
				}
			}
		}
		auto circle_queue = pos_circles(frame);
		sf::Event event;
		/* fit to text press switch */
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
			frame.set_fit_to_text(false);
		else
			frame.set_fit_to_text(true);
		
		/* Whatever is pressed, it should make no difference */
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
			frame.set_end_before_end(true);
		else
			frame.set_end_before_end(false);
		
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
			frame.set_end(sf::Vector2f(80+10, 160+10));
		else
			frame.set_end(sf::Vector2f(80, 160));
		 
		while(win.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				win.close();
		}
		 
		win.clear();
		frame.draw();
		for (auto x : circle_queue)
		{
			win.draw(x);
		}
		win.display();
	}
}

void game(sf::Font& font, sf::RenderWindow &win)
{
	class Player
	{
		unsigned int health = 100;
		sf::Vector2i position();
	};
	/* The grid of all visible objects
	 * In a "real" game, the actual, universal grid
	 * of game objects would be stored somewhere else
	 * TODO: Read up on how to arrange different classes
	 * in that grid
	 */
	std::vector<std::vector<int>> visi_grid;
	/* Window Size (should actually be continually updated) */
	sf::Vector2u win_size = win.getSize();
	/* Game View window */
	fr::Frame gv(win, font, 32, sf::Vector2f(0.f, 0.f),
			sf::Vector2f(win_size.x - 150, win_size.y));
	/* Sidebar */
	fr::Frame sb(win, font, 32, sf::Vector2f(win_size.x - 150, 0),
			sf::Vector2f(win_size.x, win_size.y));
	gv.set_frame_bg(sf::Color(54, 17, 7));
	sb.set_frame_bg(sf::Color(255, 0, 0));
	while (win.isOpen())
	{
		sf::Event event;
		while(win.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				win.close();
		}
		gv.draw();
		/* Draw sidebar OVER gameview to avoid overlapping at the
		 * edges of the two frames
		 */
		sb.draw();
		win.display();
	}
}


int main(int argc, char* argv[])
{
	sf::RenderWindow win(sf::VideoMode(1280, 720), "EXPECT UNIT TESTS");
	/* Really janky and makeshift testing suite */
	int ipt = 0;
	ipt = atoi(argv[1]);
	 
	/* Put this after so the program crashes
	 * before a window is initialized if the input is not
	 * a number;
	 * Top notch software development, I know
	 */
	sf::Font font;
	font.loadFromFile("font.otb");
	
	switch(ipt)
	{
		case 1:
			big_move_and_size(font, win);
			break;
		case 2:
			irregular_positions(font, win);
			break;
		case 3:
			move(font, win);
			break;
		case 4:
			even(font, win);
			break;
		case 5:
			game(font, win);
			break;
		default:
			std::cout << "Unit test id invalid, running a default" << std::endl;
			big_move_and_size(font, win);
			break;
	}
	return 0;
	
}
