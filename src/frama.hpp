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
		unsigned int size;
		sf::Color fill;
		sf::Color ol; // outline
		unsigned int ol_thickness;
		sf::Color bg;
		uint32_t style;
		 
		ObjRep
		(
			std::wstring my_ch,
			sf::Color my_fill = sf::Color(255, 255, 255, 0),
			sf::Color my_bg = sf::Color(0, 0, 0, 0), 
			unsigned int my_size = 2, 
			sf::Color my_ol = sf::Color(0, 0, 0, 0), 
			unsigned int my_ol_thickness = 0,
			uint32_t my_style = 0
		)
		{
			ch = my_ch[0];
			size = my_size; fill = my_fill;
			ol = my_ol; ol_thickness = my_ol_thickness;
			bg = my_bg; style = my_style;
		}
	};
	 
	class Frame
	{
		public:
			Frame(sf::RenderWindow& my_win, sf::Font& my_font, 
				int my_font_size,
				std::pair<float, float> my_ori, 
				std::pair<float, float> my_end)
			{
				win = &my_win;
				font = &my_font;
				font_size = my_font_size;
				origin = my_ori;
				end = my_end;
			}
			 
			std::wstring get_char(int x, int y);
			void set_char(ObjRep rep, int x, int y);
			 
			sf::Color get_frame_bg();
			void set_frame_bg(sf::Color col);
			
			std::pair<float, float> get_origin();
			int set_origin(std::pair<float, float>);
			
			int draw();
		private:
			std::vector<std::vector<ObjRep>> grid;
			sf::RenderWindow* win;
			sf::Font* font; int font_size = 32;
			/* Fill the background of the frame;
			 * Useful for status bars and stuff
			 * that should not be obstructed by other frames
			 */
			sf::Color frame_bg_col = sf::Color(0, 0, 0, 0);
			/* Top left beginning of text */
			std::pair<float, float> origin;
			/* UNUSED YET */
			std::pair<float, float> end;
	};
};
