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
 
int fr::Frame::draw()
{
	sf::RectangleShape frame_bg(sf::Vector2f(100.f, 100.f));
	frame_bg.setFillColor(frame_bg_col);
	frame_bg.setPosition(origin.first, origin.second);
	win->draw(frame_bg);
	for (int i = 0; i < grid.size(); i++)
	{
		for (int j = 0; j < grid[i].size(); j++)
		{
			fr::ObjRep ch = grid[i][j];
			sf::Text text;
			text.setFont(*font); /* Font of Frame */
			text.setCharacterSize(font_size);
			text.setString(ch.ch);
			text.setFillColor(ch.fill);
			text.setOutlineColor(ch.ol);
			text.setOutlineThickness(ch.ol_thickness);
			text.setStyle(ch.style);
			text.setPosition(i*24+origin.first, j*24+origin.second);
			text.scale(ch.size, ch.size);
			win->draw(text);
		}
	}
	return 0;
}

std::pair<float, float> fr::Frame::get_origin()
{
	return origin;
}

int fr::Frame::set_origin(std::pair<float, float> my_ori)
{
	origin = my_ori;
	return 0;
}
