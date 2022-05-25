#include <iostream>
#include "frama.hpp"
#include "const.h"


void fr::Frame::set_char(ObjRep rep, int x, int y)
{
	if (rep.ch.size() > 1)
		throw ERR_OVERFLOW;
	/* Empty text to shove into tiles that are left empty */
	GridObj def;
	def.t.setFont(*font);
	def.t.setCharacterSize(font_size);
	def.t.setString(L" ");
	while (y >= grid.size())
		grid.push_back(std::vector<GridObj>());
	while (x >= grid[y].size())
		grid[y].push_back(def);
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
	sf::RectangleShape frame_bg;
	frame_bg.setPosition(origin.x, origin.y);
	if (!fit_to_text)
		frame_bg.setSize(sf::Vector2f(end.x-origin.x, end.y-origin.y));
	else
	{
		float h_x = 0.f, h_y = 0.f;
		int i = 0, j = 0;
		while (grid[i][j].r.getPosition().x < end.x
				|| grid[i][j].r.getPosition().y < end.y)
		{
			j++;
			if (j >= grid[i].size()-1)
			{
				j = 0;
				i++;
			}
			std::cout << i << " " << j << "\n";
		}
		h_x = grid[i][j].r.getPosition().x;
		h_y = grid[i][j].r.getPosition().y;
		frame_bg.setSize(sf::Vector2f(h_x-origin.x, h_y-origin.y));
	}
	frame_bg.setFillColor(frame_bg_col);
	win->draw(frame_bg);
	for (int i = 0; i<text_queue.size(); i++)
	{
		if(bg_queue[i].getPosition().x < end.x && bg_queue[i].getPosition().y < end.y)
		{
			win->draw(text_queue[i]);
			win->draw(bg_queue[i]);
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
}

void fr::Frame::set_origin(sf::Vector2f my_ori)
{
	origin = my_ori;
	for (int y = 0; y < grid.size(); y++)
	{
		for (int x = 0; x < grid[y].size(); x++)
		{
			sf::Rect<float> lcl = grid[y][x].t.getLocalBounds();
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
}
