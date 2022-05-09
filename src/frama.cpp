#include "frama.hpp"
#include <iostream>

void fr::Frame::set_char(ObjRep rep, int x, int y)
{
	while (y >= grid.size())
		grid.push_back(std::vector<fr::ObjRep>());
	while (x >= grid[y].size())
	{
		sf::Font font;
		fr::ObjRep rep(L" ", font);
		grid[y].push_back(rep);
	}
	grid[y][x] = rep;
}
 
int fr::Frame::draw()
{
	for (int i = 0; i < grid.size(); i++)
	{
		for (int j = 0; j < grid[i].size(); j++)
		{
			fr::ObjRep ch = grid[i][j];
			sf::Text text;
			text.setString(ch.ch);
			text.setFont(ch.font);
			text.setCharacterSize(ch.size);
			text.setFillColor(ch.fill);
			text.setOutlineColor(ch.ol);
			text.setOutlineThickness(ch.ol_thickness);
			text.setStyle(ch.style);
			text.setPosition(i*24, j*24);
			win->draw(text);
		}
	}
	return 0;
}
