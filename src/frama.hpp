// Library to manage *everything* low-level rendering related
/* - At it's core it should just provide a way to render grids filled
 *   with colored letters through sfml;
 * - Should be able to be used standalone for other projects
 */
#pragma once

#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

namespace fr
{
	struct ObjRep
	{
		std::wstring ch;
		sf::Font font;
		unsigned int size;
		sf::Color fill;
		sf::Color ol; // outline
		unsigned int ol_thickness;
		sf::Color bg;
		uint32_t style;
		ObjRep
		(
			std::wstring my_ch, sf::Font my_font,
			sf::Color my_fill = sf::Color(255, 255, 255, 0),
			sf::Color my_bg = sf::Color(0, 0, 0, 0), 
			unsigned int my_size = 20, 
			sf::Color my_ol = sf::Color(0, 0, 0, 0), 
			unsigned int my_ol_thickness = 0,
			uint32_t my_style = 0
		)
		{
			ch = my_ch; font = my_font;
			size = my_size; fill = my_fill;
			ol = my_ol; ol_thickness = my_ol_thickness;
			bg = my_bg; style = my_style;
		}
	};
	 
	class Frame
	{
		public:
			Frame(sf::RenderWindow& my_win)
			{
				win = &my_win;
			}
			 
			std::wstring get_char(int x, int y);
			void set_char(ObjRep rep, int x, int y);
			
			int draw();
		private:
			std::vector<std::vector<ObjRep>> grid;
			sf::RenderWindow* win;
	};
};
