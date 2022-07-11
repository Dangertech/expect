#include <iostream>
#include "cmath"
#include "frama.hpp"
#include "const.h"

fr::ObjRep fr::Frame::get_char(int x, int y)
{
	auto grid_size = get_grid_size();
	if (x > grid_size.x || y > grid_size.y)
		throw ERR_OVERFLOW;
	GridObj th = grid[y][x];
	ObjRep ret;
	ret.ch = th.t.getString();
	ret.size_mod = th.size_mod;
	ret.fill = th.t.getFillColor();
	ret.ol = th.t.getOutlineColor();
	ret.ol_thickness = th.t.getOutlineThickness();
	ret.bg = th.r.getFillColor();
	ret.style = th.t.getStyle();
	return ret;
}

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
	 
	/* To support even scaling of individual characters,
	 * the origin is set to the center of the character;
	 * To keep the correct position, however, this is 
	 * evened out when calculating the character position
	 */
	text.setOrigin(sref.width/2, sref.height/2);
	
	float x_top = x*(sref.width*standard_scale) + origin.x;
	float y_top = y*(sref.height*standard_scale) + origin.y;
	text.setPosition(x_top + sref.width*standard_scale/2, 
			y_top + sref.height*standard_scale/2);
	text.setScale(standard_scale * rep.size_mod, standard_scale * rep.size_mod);
	grid[y][x].size_mod = rep.size_mod;
	grid[y][x].t = text;
	
	/* Manage background */
	sf::RectangleShape bg(sf::Vector2f(sref.width*standard_scale*rep.size_mod, 
				sref.height*standard_scale*rep.size_mod));
	bg.setPosition(x_top, y_top);
	bg.setFillColor(rep.bg);
	grid[y][x].r = bg;
	
}

int fr::Frame::print(std::wstring input, int x, int y, 
		fr::ObjRep rep, bool autobreak)
{
	auto g_size = get_grid_size();
	if (x > g_size.x)
		throw ERR_OVERFLOW;
	/* Simply set a sequence of chars
	 * Yeah, functions we can depend on!
	 */
	int my_x = x;
	int my_y = y;
	for (int i = 0; i<input.size(); i++)
	{
		if (my_y > g_size.y-1)
			return -1;
		/* Manual newline detected */
		if (input[i] == '\n')
		{
			my_x = x;
			my_y++;
			continue;
		}
		ObjRep this_rep = rep;
		this_rep.ch = input[i];
		 
		try
		{
			set_char(this_rep, my_x, my_y);
		}
		catch (int e)
		{
			std::cerr << "fr::Frame::print: set_char threw error " << e
				<< "; This should not be possible. Passing this error on." << std::endl;
			throw e;
		}
		
		my_x++;
		if (my_x > g_size.x-1)
		{
			my_x = x;
			my_y++;
		}
	}
	return my_x-x;
}


void fr::Frame::draw()
{
	if (grid.size() == 0)
		throw ERR;
	/* Queues to be filled with characters */
	std::vector<sf::Text*> text_queue;
	std::vector<sf::RectangleShape*> bg_queue;
	for (int y = 0; y < grid.size(); y++)
	{
		for (int x = 0; x < grid[y].size(); x++)
		{
			text_queue.push_back(&grid[y][x].t);
			bg_queue.push_back(&grid[y][x].r);
			
		}
	}
	/* Set up frame background */
	sf::RectangleShape frame_bg;
	frame_bg.setPosition(origin.x, origin.y);
	if (!fit_to_text)
	{
		frame_bg.setSize(sf::Vector2f(end.x-origin.x, end.y-origin.y));
	}
	else
	{
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
			brpos.x = bg_queue[i]->getPosition().x + sref.width;
			brpos.y = bg_queue[i]->getPosition().y + sref.height;
			if (brpos.x <= end.x && brpos.y <= end.y)
			{
				win->draw(*text_queue[i]);
				win->draw(*bg_queue[i]);
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
			if(bg_queue[i]->getPosition().x < end.x 
					&& bg_queue[i]->getPosition().y < end.y)
			{
				win->draw(*text_queue[i]);
				win->draw(*bg_queue[i]);
			}
		}
	}
}

void fr::Frame::clear()
{
	grid.clear();
	fill_grid();
}

void fr::Frame::set_standard_scale(float scale)
{
	if (scale <= 0)
		throw ERR_OVERFLOW;
	standard_scale = scale;
	fill_grid();
	for (int y = 0; y < grid.size(); y++)
	{
		for (int x = 0; x < grid[y].size(); x++)
		{
			float x_top = x*(sref.width*standard_scale) + origin.x;
			float y_top = y*(sref.height*standard_scale) + origin.y;
			grid[y][x].t.setScale(standard_scale * grid[y][x].size_mod, 
					standard_scale * grid[y][x].size_mod);
			grid[y][x].t.setPosition(x_top + sref.width*standard_scale/2,
					y_top + sref.height*standard_scale/2);
			grid[y][x].r.setSize(sf::Vector2f(sref.width*standard_scale*grid[y][x].size_mod,
					sref.height*standard_scale*grid[y][x].size_mod));
			grid[y][x].r.setPosition(x_top + sref.width*standard_scale/2,
					y_top + sref.height*standard_scale/2);
		}
	}
}

void fr::Frame::set_origin(sf::Vector2i my_ori)
{
	origin = my_ori;
	fill_grid();
	for (int y = 0; y < grid.size(); y++)
	{
		for (int x = 0; x < grid[y].size(); x++)
		{
			sf::Rect<float> lcl(0,0, get_standard_char_size().x, 
					get_standard_char_size().y);
			float x_top = x*(lcl.width*standard_scale) + origin.x;
			float y_top = y*(lcl.height*standard_scale) + origin.y;
			grid[y][x].t.setPosition(x_top + lcl.width*standard_scale/2, 
					y_top + lcl.height*standard_scale/2);
			grid[y][x].r.setPosition(x_top, y_top);
		}
	}
}

void fr::Frame::set_end(sf::Vector2i my_end)
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
	return sf::Vector2f(sref.width, sref.height);
}

sf::Vector2<int> fr::Frame::get_grid_size()
{
	sf::Vector2f char_size = get_char_size();
	if (origin.x > end.x || origin.y > end.y)
		return sf::Vector2<int>(0, 0);
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
		sf::Vector2f size((end.x-origin.x)/char_size.x, 
				(end.y-origin.y)/char_size.y);
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

sf::Vector2<int> fr::Frame::get_char_at(int win_x, int win_y)
{
	sf::Vector2f char_size = get_char_size();
	float x = (win_x-origin.x)/char_size.x, y = (win_y-origin.y)/char_size.y;
	
	/* Check if the grid is this large */
	sf::Vector2<int> grid_size = get_grid_size();
	if (x > grid_size.x || y > grid_size.y || x < 0 || y < 0)
		throw ERR;
	/* Truncated automatically*/
	return sf::Vector2<int>(x, y);
}

void fr::Frame::fill_grid()
{
	/* Default object to shove into empty spaces */
	GridObj def;
	def.t.setFont(*font);
	def.t.setCharacterSize(font_size);
	def.t.setString(L" ");
	def.t.setOrigin(sref.width/2, sref.height/2);
	def.r.setFillColor(sf::Color(0,0,0,0));
	 
	sf::Vector2<int> goal_size = get_grid_size();
	while (grid.size() < goal_size.y)
	{
		grid.push_back(std::vector<GridObj>());
	}
	 
	for (int y = 0; y < grid.size(); y++)
	{
		while (grid[y].size() < goal_size.x)
		{
			float x_top = grid[y].size() * (sref.width*standard_scale) + origin.x;
			float y_top = y * (sref.height*standard_scale) + origin.y;
			def.t.setPosition(x_top + sref.width*standard_scale/2, 
					y_top + sref.height*standard_scale/2);
			def.r.setPosition(x_top, y_top);
			def.r.setSize(sf::Vector2f(sref.width*standard_scale, 
					sref.height*standard_scale));
			grid[y].push_back(def);
		}
	}
}

void fr::Frame::set_size_ref()
{
	sf::Text text;
	text.setFont(*font);
	text.setCharacterSize(font_size);
	text.setString(L"#");
	sref = text.getLocalBounds();
}
