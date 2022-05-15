#include <iostream>
#include "frama.hpp"
#include "const.h"


void fr::Frame::set_char(ObjRep rep, int x, int y)
{
	if (rep.ch.size() > 1)
		throw ERR_OVERFLOW;
	while (y >= grid.size())
		grid.push_back(std::vector<fr::ObjRep>());
	while (x >= grid[y].size())
	{
		sf::Font font;
		fr::ObjRep rep(L" ");
		grid[y].push_back(rep);
	}
	grid[y][x] = rep;
}

void fr::Frame::draw()
{
	if (grid.size() == 0)
		throw ERR;
	/* Queues to be filled with characters */
	std::vector<sf::Text> text_queue;
	std::vector<sf::RectangleShape> bg_queue;
	int posfix = 0; // Bodge for a really weird bug
	/* Stores the maximum size of the frame */
	int max_left = 0, max_down = 0; 
	for (int y = 0; y < grid.size(); y++)
	{
		for (int x = 0; x < grid[y].size(); x++)
		{
			/* Set up general properties */
			fr::ObjRep ch = grid[y][x];
			sf::Text text;
			text.setFont(*font); /* Font of Frame */
			text.setCharacterSize(font_size);
			text.setString(ch.ch);
			text.setFillColor(ch.fill);
			text.setOutlineColor(ch.ol);
			text.setOutlineThickness(ch.ol_thickness);
			text.setStyle(ch.style);
			 
			/* Transformations */
			sf::Rect<float> lcl = text.getLocalBounds();
			/* To support even scaling of individual characters,
			 * the origin is set to the center of the character;
			 * To keep the correct position, however, this is 
			 * evened out when calculating the character position
			 */
			text.setOrigin(lcl.width/2, lcl.height/2);
			/* character column/row * char width * extra scaling on frame + 
			 * frame origin displacement + local origin fix
			 */
			float x_pos = x*(lcl.width*standard_scale) 
				+ origin.x + lcl.width*standard_scale/2;
			float y_pos = y*(lcl.height*standard_scale) 
				+ origin.y + lcl.height*standard_scale/2 - posfix;
			/* + 2 is a safe margin to prevent flickering when
			 * moving frames;
			 * Also, it's a steady play with that origin; Remove the origin
			 * point from the position again...
			 */
			if (x_pos-lcl.width/2 > win->getSize().x 
				|| y_pos-lcl.height/2 > win->getSize().y
				|| x_pos > end.x + 2
				|| y_pos > end.y + 2)
				continue; /* Character won't be visible anyways */
			text.setPosition(x_pos, y_pos);
			/* To fix a bug that offsets the y position of the text
			 * seemingly at random; Probably I'm just too stupid to fix
			 * this bug so this is a quick bodge
			 */
			if (y == 0 && x == 0)
			{
				posfix = text.getGlobalBounds().top - origin.y;
				text.setPosition(x_pos, y_pos - posfix);
			}
			text.setScale(standard_scale * ch.size_mod, standard_scale * ch.size_mod);
			
			 
			/* Set up background shape (from GlobalBounds) */
			sf::Rect<float> bnds = text.getGlobalBounds();
			sf::RectangleShape bg(sf::Vector2f(bnds.width, bnds.height));
			bg.setFillColor(ch.bg);
			bg.setPosition(bnds.left, bnds.top);
			
			/* Schedule bg and character for drawing */
			text_queue.push_back(text);
			bg_queue.push_back(bg);
			
			/* For the frame bg if fit_to_text is true */
			if (bnds.left + bnds.width - origin.x > max_left)
				max_left = bnds.left + bnds.width - origin.x;
			if (bnds.top + bnds.height - origin.y > max_down)
				max_down = bnds.top + bnds.height - origin.y;
			 
		}
	}
	 
	/* Determine correct frame background from filled queue */
	sf::RectangleShape frame_bg;
	if (!fit_to_text)
		frame_bg.setSize(sf::Vector2f(end.x-origin.x, end.y-origin.y));
	else
		frame_bg.setSize(sf::Vector2f(max_left, max_down));
	frame_bg.setFillColor(frame_bg_col);
	frame_bg.setPosition(origin.x, origin.y);
	win->draw(frame_bg);
	
	/* Draw everything in the queues */
	for (int i = 0; i<text_queue.size(); i++)
	{
		win->draw(bg_queue[i]);
		win->draw(text_queue[i]);
	}
}

void fr::Frame::set_standard_scale(float scale)
{
	if (scale <= 0)
		throw ERR_OVERFLOW;
	standard_scale = scale;
}

void fr::Frame::set_origin(sf::Vector2f my_ori)
{
	origin = my_ori;
}

void fr::Frame::set_end(sf::Vector2f my_end)
{
	end = my_end;
}
