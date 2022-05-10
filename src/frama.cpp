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
	sf::RectangleShape frame_bg(sf::Vector2f(100.f, 100.f));
	frame_bg.setFillColor(frame_bg_col);
	frame_bg.setPosition(origin.first, origin.second);
	win->draw(frame_bg);
	if (grid.size() == 0)
		throw ERR;
	for (int y = 0; y < grid.size(); y++)
	{
		for (int x = 0; x < grid[y].size(); x++)
		{
			// Set up general properties
			fr::ObjRep ch = grid[y][x];
			sf::Text text;
			text.setFont(*font); /* Font of Frame */
			text.setCharacterSize(font_size);
			text.setString(ch.ch);
			text.setFillColor(ch.fill);
			text.setOutlineColor(ch.ol);
			text.setOutlineThickness(ch.ol_thickness);
			text.setStyle(ch.style);
			 
			// Transformations
			sf::Rect<float> lcl = text.getLocalBounds();
			/* To support even scaling of individual characters,
			 * the origin is set to the center of the character;
			 * To keep the correct position, however, this is 
			 * evened out when calculating the character position
			 */
			text.setOrigin(lcl.width/2, lcl.height/2);
			text.setPosition(
				x*(15*standard_scale)+ origin.first + lcl.width/2,
				y*(32*standard_scale)+ origin.second + lcl.height/2
			);
			text.scale(standard_scale * ch.size_mod, standard_scale * ch.size_mod);
			
			// Set up background shape (from GlobalBounds)
			sf::Rect<float> bnds = text.getGlobalBounds();
			sf::RectangleShape bg(sf::Vector2f(bnds.width, bnds.height));
			bg.setFillColor(ch.bg);
			bg.setPosition(bnds.left, bnds.top);

			// Draw background and then text over it
			win->draw(bg);
			win->draw(text);
		}
	}
}

void fr::Frame::set_origin(std::pair<float, float> my_ori)
{
	/* TODO: Check if origin is in window */
	origin = my_ori;
}

void fr::Frame::set_standard_scale(float scale)
{
	if (scale <= 0)
		throw ERR_OVERFLOW;
	standard_scale = scale;
}
