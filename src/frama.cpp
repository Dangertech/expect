#include <iostream>
#include "cmath"
#include "frama.hpp"
#include "const.h"

fr::ObjRep fr::Frame::get_char(int x, int y)
{
	if (y > get_grid_size().y -1 || x > get_grid_size().x -1)
		throw ERR_OVERFLOW;
	if (grid.find((unsigned long)x << 32 | (unsigned long) y) == grid.end())
	{
		return EMPTY;
	}
	else
	{
		return grid.at((unsigned long)x << 32 | (unsigned long) y).refobj;
	}
}

void fr::Frame::set_char(ObjRep rep, int x, int y)
{

	if (y > get_grid_size().y || x > get_grid_size().x)
	{
		throw ERR_OVERFLOW;
	}
	if (rep == fr::EMPTY)
	{
		grid.erase((unsigned long)x << 32 | (unsigned long) y);
		return;
	}
	GridObj newobj; 
	newobj.refobj = rep;
	newobj.s.setTexture(*font_txt);
	newobj.s.setTextureRect(font->getGlyph(rep.ch, font_size, rep.bold).textureRect); 
	newobj.s.setColor(rep.fill);
	newobj.s.setScale(standard_scale * rep.size_mod, standard_scale * rep.size_mod);
	newobj.size_mod = rep.size_mod;
	
	float x_top = x*(sref.width*standard_scale) + origin.x + margin.x*x;
	float y_top = y*(sref.height*standard_scale) + origin.y + margin.y*y;
	newobj.s.setPosition(sf::Vector2f(x_top, y_top));
	 
	/* Manage background */
	sf::RectangleShape bg(sf::Vector2f(sref.width*standard_scale*rep.size_mod, 
				sref.height*standard_scale*rep.size_mod));
	bg.setPosition(x_top, y_top);
	bg.setFillColor(rep.bg);
	newobj.r = bg;
	grid[(unsigned long)x << 32 | (unsigned long) y] = newobj;
	to_update = true;
	
}

sf::Vector2i fr::Frame::print(std::wstring input, int x, int y, 
		fr::ObjRep rep, bool autobreak, int max_x, int max_y, bool dry)
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
			return {my_x, my_y-1};
		/* Manual newline detected */
		if (input[i] == '\n')
		{
			my_x = x;
			my_y++;
			continue;
		}
		ObjRep this_rep = rep;
		this_rep.ch = input[i];
		 
		if (!dry)
		{
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
		}
		
		my_x++;
		if (my_x > g_size.x-1 || (max_x != -1 && my_x > max_x))
		{
			if (autobreak)
			{
				my_x = x;
				my_y++;
			}
			else
				return {my_x, my_y};
		}
		if (max_y != -1 && my_y > max_y)
			return {my_x, my_y};
	}
	return {my_x, my_y};
	to_update = true;
}


int fr::Frame::draw(sf::Shader* shad)
{
	if (to_update == false)
		return 0;
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
			chr_size.x += margin.x; chr_size.y += margin.y;
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
		for (std::pair<unsigned long, GridObj> i : grid)
		{
			sf::Vector2f brpos;
			brpos.x = i.second.r.getPosition().x + sref.width;
			brpos.y = i.second.r.getPosition().y + sref.height;
			if (brpos.x <= end.x && brpos.y <= end.y)
			{
				win->draw(i.second.r);
				win->draw(i.second.s, shad);
			}
		}
	}
	else
	{
		/* Here, the top left is checked if
		 * it is beyond the end
		 */
		for (std::pair<unsigned long, GridObj> i : grid)
		{
			/* Use the backgrounds because their origin is still 0,0 */
			if (i.second.r.getPosition().x < end.x 
					&& i.second.r.getPosition().y < end.y)
			{
				win->draw(i.second.r);
				win->draw(i.second.s, shad);
			}
		}
	}
	to_update = false;
	return 1;
}

void fr::Frame::clear()
{
	grid.clear();
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
	for (std::pair<unsigned long, GridObj> i : grid)
	{
		float x_top = (i.first >> 32)*(sref.width*standard_scale) + origin.x;
		float y_top = ((int) i.first)*(sref.height*standard_scale) + origin.y;
		i.second.s.setScale(standard_scale * i.second.size_mod, 
				standard_scale * i.second.size_mod);
		i.second.s.setPosition(x_top + sref.width*standard_scale/2,
				y_top + sref.height*standard_scale/2);
		i.second.r.setSize(sf::Vector2f(sref.width*standard_scale*i.second.size_mod,
				sref.height*standard_scale*i.second.size_mod));
		i.second.r.setPosition(x_top + sref.width*standard_scale/2,
				y_top + sref.height*standard_scale/2);
	}
	to_update = true;
}

void fr::Frame::set_origin(sf::Vector2i my_ori)
{
	origin = my_ori;
	reserve_grid();
	for (std::pair<unsigned long, GridObj> i : grid)
	{
			sf::Rect<float> lcl(0,0, get_standard_char_size().x, 
					get_standard_char_size().y);
			float x_top = (i.first >> 32)*(lcl.width*standard_scale) + origin.x;
			float y_top = (int)i.first*(lcl.height*standard_scale) + origin.y;
			i.second.s.setPosition(x_top + lcl.width*standard_scale/2, 
					y_top + lcl.height*standard_scale/2);
			i.second.r.setPosition(x_top, y_top);
	}
	to_update = true;
}

void fr::Frame::set_end(sf::Vector2i my_end)
{
	end = my_end;
	reserve_grid();
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
	char_size.x += margin.x;
	char_size.y += margin.y;
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
	if (x > get_grid_size().x-1 || y > get_grid_size().y-1)
		throw ERR_OVERFLOW;
	/* Get the selected GridObj */
	switch(point)
	{
		case CENTER:
			break;
		default:
			std::cout << "Ooops! This character point is not yet implemented...\n";
			break;
	}
	return pos;
}

sf::Vector2<int> fr::Frame::get_char_at(int win_x, int win_y)
{
	sf::Vector2f char_size = get_char_size();
	char_size.x += margin.x; char_size.y += margin.y;
	float x = (win_x-origin.x)/char_size.x, y = (win_y-origin.y)/char_size.y;
	
	/* Check if the grid is this large */
	sf::Vector2<int> grid_size = get_grid_size();
	if (x > grid_size.x || y > grid_size.y || x < 0 || y < 0)
		throw ERR;
	/* Truncated automatically*/
	return sf::Vector2<int>(x, y);
}

void fr::Frame::reserve_grid()
{
	sf::Vector2i grid_size = get_grid_size();
	if (grid.size() > grid_size.x*grid_size.y)
		return;
	grid.reserve(grid_size.y*grid_size.x);
}

void fr::Frame::set_size_ref()
{
	sf::Sprite s;
	s.setTexture(*font_txt);
	s.setTextureRect(font->getGlyph(L'#', font_size, false).textureRect);
	sref = s.getLocalBounds();
}

namespace fr{
namespace anim
{
	void slide_down(float t, fr::Frame& fr, int slices, sf::Color slice_color, bool reverse)
	{
		if (t > 1 || t < 0)
			return;
		if (!reverse)
		{
			float lid_pos = fr.get_grid_size().y*(t*t);
			for (int y = lid_pos-slices; y<lid_pos; y++)
			{
				if (y<0)
					continue;
				for (int x = 0; x<fr.get_grid_size().x; x++)
				{
					sf::Color col = slice_color;
					col.a = ((y-lid_pos-slices+1)/slices)*255;
					ObjRep th = fr.get_char(x, y);
					th.bg = col; 
					fr.set_char(th, x, y);
				}
			}
			for (int y = lid_pos;y<fr.get_grid_size().y; y++)
			{
				for (int x = 0; x<fr.get_grid_size().x; x++)
				{
					fr.set_char(EMPTY, x, y);
				}
			}
		}
		else
		{
			float lid_pos = fr.get_grid_size().y*(1-(t*t))-1;
			for (int y = lid_pos+slices; y>lid_pos; y--)
			{
				if (y>=fr.get_grid_size().y || y<0)
					continue;
				for (int x = 0; x<fr.get_grid_size().x; x++)
				{
					sf::Color col = slice_color;
					col.a = 1-(((y-lid_pos)/slices)*255);
					ObjRep th = fr.get_char(x, y);
					th.bg = col; 
					fr.set_char(th, x, y);
				}
			}
			for (int y = 0;y<lid_pos; y++)
			{
				for (int x = 0; x<fr.get_grid_size().x; x++)
				{
					fr.set_char(EMPTY, x, y);
				}
			}
		}
		
	}
}}
