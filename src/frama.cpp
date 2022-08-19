#include <iostream>
#include "cmath"
#include "frama.hpp"
#include "const.h"

fr::ObjRep fr::Frame::get_char(int x, int y)
{
	if (y > grid.size() -1 || x > grid[y].size()-1)
		throw ERR_OVERFLOW;
	return grid[y][x].refobj;
}

void fr::Frame::set_char(ObjRep rep, int x, int y)
{
	if (get_char(x, y) == rep)
		return;
	
	fill_grid();
	
	if (y > grid.size()-1 || x > grid[y].size()-1)
	{
		throw ERR_OVERFLOW;
	}
	 
	grid[y][x].refobj = rep;
	grid[y][x].s.setTexture(*font_txt);
	grid[y][x].s.setTextureRect(font->getGlyph(rep.ch, font_size, rep.bold).textureRect); 
	grid[y][x].s.setColor(rep.fill);
	/* To support even scaling of individual characters,
	 * the origin is set to the center of the character;
	 * To keep the correct position, however, this is 
	 * evened out when calculating the character position
	 */
	grid[y][x].s.setOrigin(sref.width/2, sref.height/2);
	
	float x_top = x*(sref.width*standard_scale) + origin.x;
	float y_top = y*(sref.height*standard_scale) + origin.y;
	grid[y][x].s.setPosition(x_top + sref.width*standard_scale/2, 
			y_top + sref.height*standard_scale/2);
	grid[y][x].s.setScale(standard_scale * rep.size_mod, standard_scale * rep.size_mod);
	grid[y][x].size_mod = rep.size_mod;
	
	/* Manage background */
	sf::RectangleShape bg(sf::Vector2f(sref.width*standard_scale*rep.size_mod, 
				sref.height*standard_scale*rep.size_mod));
	bg.setPosition(x_top, y_top);
	bg.setFillColor(rep.bg);
	grid[y][x].r = bg;
	to_update = true;
	
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
	to_update = true;
}


int fr::Frame::draw(sf::Shader* shad)
{
	if (to_update == false)
		return 0;
	if (grid.size() == 0)
		throw ERR;
	/* Queues to be filled with characters */
	std::vector<sf::Sprite*> sprite_queue;
	std::vector<sf::RectangleShape*> bg_queue;
	for (int y = 0; y < grid.size(); y++)
	{
		for (int x = 0; x < grid[y].size(); x++)
		{
			sprite_queue.push_back(&grid[y][x].s);
			bg_queue.push_back(&grid[y][x].r);
			
		}
	}
	/* Set up frame background */
	sf::Vector2f bg_pos = frame_bg.getPosition();
	sf::Vector2f bg_size = frame_bg.getSize();
	if (bg_pos.x != origin.x || bg_pos.y != origin.y 
			|| bg_size.x != end.x-origin.x || bg_size.y != end.y-origin.y
			|| frame_bg.getFillColor() != frame_bg_col)
	{
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
	}
	
	 
	/* Draw queues */
	if (end_before_end)
	{
		/* Here, the bottom right is checked if 
		 * it is beyond the end
		 */
		for (int i = 0; i<sprite_queue.size(); i++)
		{
			sf::Vector2f brpos;
			brpos.x = bg_queue[i]->getPosition().x + sref.width;
			brpos.y = bg_queue[i]->getPosition().y + sref.height;
			if (brpos.x <= end.x && brpos.y <= end.y)
			{
				win->draw(*sprite_queue[i], shad);
				win->draw(*bg_queue[i], shad);
			}
		}
	}
	else
	{
		/* Here, the top left is checked if
		 * it is beyond the end
		 */
		for (int i = 0; i<sprite_queue.size(); i++)
		{
			/* Use the backgrounds because their origin is still 0,0 */
			if(bg_queue[i]->getPosition().x < end.x 
					&& bg_queue[i]->getPosition().y < end.y)
			{
				win->draw(*sprite_queue[i], shad);
				win->draw(*bg_queue[i], shad);
			}
		}
	}
	to_update = false;
	return 1;
}

void fr::Frame::clear()
{
	grid.clear();
	fill_grid();
	to_update = true;
}

bool fr::Frame::get_update()
{
	return to_update;
}

void fr::Frame::force_update()
{
	to_update = true;
}

void fr::Frame::set_standard_scale(float scale)
{
	if (scale <= 0)
		throw ERR_OVERFLOW;
	standard_scale = scale;
	reserve_grid();
	fill_grid();
	for (int y = 0; y < grid.size(); y++)
	{
		for (int x = 0; x < grid[y].size(); x++)
		{
			float x_top = x*(sref.width*standard_scale) + origin.x;
			float y_top = y*(sref.height*standard_scale) + origin.y;
			grid[y][x].s.setScale(standard_scale * grid[y][x].size_mod, 
					standard_scale * grid[y][x].size_mod);
			grid[y][x].s.setPosition(x_top + sref.width*standard_scale/2,
					y_top + sref.height*standard_scale/2);
			grid[y][x].r.setSize(sf::Vector2f(sref.width*standard_scale*grid[y][x].size_mod,
					sref.height*standard_scale*grid[y][x].size_mod));
			grid[y][x].r.setPosition(x_top + sref.width*standard_scale/2,
					y_top + sref.height*standard_scale/2);
		}
	}
	to_update = true;
}

void fr::Frame::set_origin(sf::Vector2i my_ori)
{
	origin = my_ori;
	reserve_grid();
	fill_grid();
	for (int y = 0; y < grid.size(); y++)
	{
		for (int x = 0; x < grid[y].size(); x++)
		{
			sf::Rect<float> lcl(0,0, get_standard_char_size().x, 
					get_standard_char_size().y);
			float x_top = x*(lcl.width*standard_scale) + origin.x;
			float y_top = y*(lcl.height*standard_scale) + origin.y;
			grid[y][x].s.setPosition(x_top + lcl.width*standard_scale/2, 
					y_top + lcl.height*standard_scale/2);
			grid[y][x].r.setPosition(x_top, y_top);
		}
	}
	to_update = true;
}

void fr::Frame::set_end(sf::Vector2i my_end)
{
	end = my_end;
	reserve_grid();
	fill_grid();
	to_update = true;
}

sf::Vector2f fr::Frame::get_char_size(int size_mod)
{
	sf::Sprite s;
	s.setTexture(*font_txt);
	s.setTextureRect(font->getGlyph(L'#', font_size, false).textureRect);
	s.setScale(standard_scale * size_mod, standard_scale * size_mod);
	sf::Rect<float> glb = s.getGlobalBounds();
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
			pos = grid[y][x].s.getPosition();
			break;
		default:
			std::cout << "Ooops! This character point is not yet implemented...\n";
			pos = grid[y][x].s.getPosition();
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
	def.s.setTexture(*font_txt);
	def.s.setTextureRect(font->getGlyph(L' ', font_size, false).textureRect);
	def.r.setFillColor(sf::Color(0,0,0,0));
	def.refobj = fr::EMPTY;
	 
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
			def.s.setPosition(x_top + sref.width*standard_scale/2, 
					y_top + sref.height*standard_scale/2);
			def.r.setPosition(x_top, y_top);
			def.r.setSize(sf::Vector2f(sref.width*standard_scale, 
					sref.height*standard_scale));
			grid[y].push_back(def);
		}
	}
	to_update = true;
}

void fr::Frame::reserve_grid()
{
	sf::Vector2i grid_size = get_grid_size();
	/* Only so the function doesn't crash, doesn't
	 * really make sense performance-wise
	 */
	if (grid.size() > 0)
	{
		if (grid.size() > grid_size.y || grid[0].size() > grid_size.x)
			return;
	}
	grid.reserve(grid_size.y);
	for (int i = 0; i<grid.size(); i++)
	{
		grid[i].reserve(grid_size.x);
	}
}

void fr::Frame::set_size_ref()
{
	sf::Sprite s;
	s.setTexture(*font_txt);
	s.setTextureRect(font->getGlyph(L'#', font_size, false).textureRect);
	sref = s.getLocalBounds();
}
