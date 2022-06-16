#include <iostream>
#include "cmath"
#include "frama.hpp"
#include "const.h"


void fr::Frame::set_char(ObjRep rep, int x, int y)
{
	if (rep.ch.size() > 1)
		throw ERR_INVALID_IPT;
	
	fill_grid();

	if (y > grid.size()-1 || x > grid[y].size()-1)
	{
		throw ERR_OVERFLOW;
	}
	 
	sf::Text text;
	text.setFont(*font);
	text.setCharacterSize(font_size);
	text.setString(rep.ch);
	text.setFillColor(rep.fill);
	text.setOutlineColor(rep.ol);
	text.setOutlineThickness(rep.ol_thickness);
	text.setStyle(rep.style);
	 
	/* Set position and scale */
	sf::Rect<float> lcl = text.getLocalBounds();
	/* To support even scaling of individual characters,
	 * the origin is set to the center of the character;
	 * To keep the correct position, however, this is 
	 * evened out when calculating the character position
	 */
	text.setOrigin(lcl.width/2, lcl.height/2);
	
	float x_top = x*(lcl.width*standard_scale) + origin.x;
	float y_top = y*(lcl.height*standard_scale) + origin.y;
	text.setPosition(x_top + lcl.width*standard_scale/2, y_top + lcl.height*standard_scale/2);
	text.setScale(standard_scale * rep.size_mod, standard_scale * rep.size_mod);
	grid[y][x].size_mod = rep.size_mod;
	grid[y][x].t = text;
	
	/* Manage background */
	sf::RectangleShape bg(sf::Vector2f(lcl.width*standard_scale*rep.size_mod, 
				lcl.height*standard_scale*rep.size_mod));
	bg.setPosition(x_top, y_top);
	bg.setFillColor(rep.bg);
	grid[y][x].r = bg;
	
}

void fr::Frame::draw()
{
	if (grid.size() == 0)
		throw ERR;
	/* Queues to be filled with characters */
	std::vector<sf::Text> text_queue;
	std::vector<sf::RectangleShape> bg_queue;
	for (int y = 0; y < grid.size(); y++)
	{
		for (int x = 0; x < grid[y].size(); x++)
		{
			text_queue.push_back(grid[y][x].t);
			bg_queue.push_back(grid[y][x].r);
			
		}
	}
	/* Set up frame background */
	sf::RectangleShape frame_bg;
	frame_bg.setPosition(origin.x, origin.y);
	if (!fit_to_text)
	{
		frame_bg.setSize(sf::Vector2f(end.x-origin.x, end.y-origin.y));
		std::cout << "Standard Shit" << std::endl;
	}
	else
	{
		std::cout << "Original shit" << std::endl;
		sf::Vector2<int> grid_size = get_grid_size();
		sf::Vector2f chr_size = get_char_size();
		frame_bg.setSize(sf::Vector2f(grid_size.x*chr_size.x,
			grid_size.y*chr_size.y));
	}
	frame_bg.setFillColor(frame_bg_col);
	win->draw(frame_bg);
	/* Draw queues */
	if (end_before_end)
	{
		/* Here, the bottom right is checked if 
		 * it is beyond the end
		 */
		for (int i = 0; i<text_queue.size(); i++)
		{
			sf::Vector2f brpos;
			sf::Rect<float> lcl = bg_queue[i].getLocalBounds();
			brpos.x = bg_queue[i].getPosition().x + lcl.width;
			brpos.y = bg_queue[i].getPosition().y + lcl.height;
			if (brpos.x <= end.x && brpos.y <= end.y)
			{
				win->draw(text_queue[i]);
				win->draw(bg_queue[i]);
			}
		}
	}
	else
	{
		/* Here, the top left is checked if
		 * it is beyond the end
		 */
		for (int i = 0; i<text_queue.size(); i++)
		{
			/* Use the backgrounds because their origin is still 0,0 */
			if(bg_queue[i].getPosition().x < end.x && bg_queue[i].getPosition().y < end.y)
			{
				win->draw(text_queue[i]);
				win->draw(bg_queue[i]);
			}
		}
	}
}

void fr::Frame::set_standard_scale(float scale)
{
	if (scale <= 0)
		throw ERR_OVERFLOW;
	standard_scale = scale;
	for (int y = 0; y < grid.size(); y++)
	{
		for (int x = 0; x < grid[y].size(); x++)
		{
			grid[y][x].t.setScale(standard_scale * grid[y][x].size_mod, 
					standard_scale * grid[y][x].size_mod);
			sf::Rect<float> lcl = grid[y][x].t.getLocalBounds();
			grid[y][x].r.setSize(sf::Vector2f(lcl.width*standard_scale*grid[y][x].size_mod,
					lcl.height*standard_scale*grid[y][x].size_mod));
		}
	}
	fill_grid();
}

void fr::Frame::set_origin(sf::Vector2f my_ori)
{
	origin = my_ori;
	fill_grid();
	for (int y = 0; y < grid.size(); y++)
	{
		for (int x = 0; x < grid[y].size(); x++)
		{
			sf::Rect<float> lcl(0,0, get_standard_char_size().x, get_standard_char_size().y);
			float x_top = x*(lcl.width*standard_scale) + origin.x;
			float y_top = y*(lcl.height*standard_scale) + origin.y;
			grid[y][x].t.setPosition(x_top + lcl.width*standard_scale/2, y_top + lcl.height*standard_scale/2);
			grid[y][x].r.setPosition(x_top, y_top);
		}
	}
}

void fr::Frame::set_end(sf::Vector2f my_end)
{
	end = my_end;
	fill_grid();
}

sf::Vector2f fr::Frame::get_char_size(int size_mod)
{
	sf::Text text;
	text.setFont(*font);
	text.setCharacterSize(font_size);
	text.setString(L"#");
	text.setScale(standard_scale * size_mod, standard_scale * size_mod);
	sf::Rect<float> glb = text.getGlobalBounds();
	return sf::Vector2f(glb.width, glb.height);
}

sf::Vector2f fr::Frame::get_standard_char_size()
{
	sf::Text text;
	text.setFont(*font);
	text.setCharacterSize(font_size);
	text.setString(L"#");
	sf::Rect<float> lcl = text.getLocalBounds();
	return sf::Vector2f(lcl.width, lcl.height);
}

sf::Vector2<int> fr::Frame::get_grid_size()
{
	sf::Vector2f char_size = get_char_size();
	if (end_before_end)
	{
		/* Truncate/Floor the result to int */
		return sf::Vector2<int>
		(
			int((end.x - origin.x)/char_size.x),
			int((end.y-origin.y)/char_size.y)
		);
	}
	else
	{
		sf::Vector2f size((end.x-origin.x)/char_size.x, (end.y-origin.y)/char_size.y);
		if (ceil(size.x) != size.x)
			size.x = ceil(size.x);
		if (ceil(size.y) != size.y)
			size.y = ceil(size.y);
		return sf::Vector2<int>
		(
			int (size.x), int(size.y)
		);
	}
}

sf::Vector2f fr::Frame::get_char_pos(int x, int y, CharPoint point)
{
	sf::Vector2f pos;
	if (y > grid.size()-1 || x > grid[grid.size()-1].size()-1)
		throw ERR_OVERFLOW;
	switch(point)
	{
		case CENTER:
			pos = grid[y][x].t.getPosition();
			break;
		default:
			std::cout << "Ooops! This character point is not yet implemented...\n";
			pos = grid[y][x].t.getPosition();
			break;
	}
	return pos;
}

void fr::Frame::fill_grid()
{
	/* Default object to shove into empty spaces */
	GridObj def;
	def.t.setFont(*font);
	def.t.setCharacterSize(font_size);
	def.t.setString(L" ");
	sf::Rect<float> dlcl(0,0,get_standard_char_size().x, get_standard_char_size().y);
	def.t.setOrigin(dlcl.width/2, dlcl.height/2);
	 
	sf::Vector2<int> goal_size = get_grid_size();
	while (grid.size() < goal_size.y)
	{
		grid.push_back(std::vector<GridObj>());
	}
	for (int y = 0; y < grid.size(); y++)
	{
		while (grid[y].size() < goal_size.x)
		{
			float x_top = grid[y].size() * (dlcl.width*standard_scale) + origin.x;
			float y_top = y * (dlcl.height*standard_scale) + origin.y;
			def.t.setPosition(x_top + dlcl.width*standard_scale/2, y_top + dlcl.height*standard_scale/2);
			def.r.setPosition(x_top, y_top);
			grid[y].push_back(def);
		}
	}
}
