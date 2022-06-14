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
	enum CharPoint {TOP, LEFT, RIGHT, DOWN, TOP_LEFT, TOP_RIGHT, 
		BOTTOM_LEFT, BOTTOM_RIGHT, CENTER};
	struct ObjRep
	{
		std::wstring ch;
		float size_mod;
		sf::Color fill;
		sf::Color ol; // outline
		unsigned int ol_thickness;
		sf::Color bg;
		uint32_t style;
		 
		ObjRep
		(
			std::wstring my_ch,
			sf::Color my_fill = sf::Color(255, 255, 255, 255),
			sf::Color my_bg = sf::Color(0, 0, 0, 0), 
			float my_size_mod = 1, 
			sf::Color my_ol = sf::Color(0, 0, 0, 0), 
			unsigned my_ol_thickness = 0,
			uint32_t my_style = 0
		)
		{
			ch = my_ch[0];
			size_mod = my_size_mod; fill = my_fill;
			ol = my_ol; ol_thickness = my_ol_thickness;
			bg = my_bg; style = my_style;
		}
	};
	 
	class Frame
	{
		public:
			Frame(sf::RenderWindow& my_win, sf::Font& my_font, 
				int my_font_size,
				sf::Vector2f my_ori, 
				sf::Vector2f my_end)
			{
				win = &my_win;
				font = &my_font;
				font_size = my_font_size;
				origin = my_ori;
				end = my_end;
				fill_grid();
			}
			 
			std::wstring get_char(int x, int y);
			void set_char(ObjRep rep, int x, int y);
			
			float get_standard_scale() {return standard_scale; }
			void set_standard_scale(float scale);
			 
			sf::Color get_frame_bg();
			void set_frame_bg(sf::Color col) {frame_bg_col = col;}
			
			sf::Vector2f get_origin() { return origin; }
			void set_origin(sf::Vector2f origin);
			
			sf::Vector2f get_end() { return end; }
			void set_end(sf::Vector2f end);
			
			bool get_fit_to_text() { return fit_to_text; }
			void set_fit_to_text(bool new_fit) {fit_to_text = new_fit;}
			
			/* Returns the bounding box of the # character with font size,
			 * standard scale and size_mod if provided;
			 * (as a representation for every character in a monospace font)
			 */
			sf::Vector2f get_char_size(int size_mod = 1);
			
			/* Returns the bounding box of the # character with
			 * the set font and font size but without additional scaling
			 */
			sf::Vector2f get_standard_char_size();
			
			/* Get the amount of possible columns and rows with the current size */
			sf::Vector2<int> get_grid_size();
			
			/* Get the position of a character on the grid, 
			 * at any of the 9 geometrical points */
			sf::Vector2f get_char_pos(int x, int y, CharPoint point = CENTER);
			
			void draw();
		private:
			struct GridObj
			{
				sf::Text t;
				sf::RectangleShape r;
				float size_mod = 1;
			};
			std::vector<std::vector<GridObj>> grid;
			sf::RenderWindow* win;
			sf::Font* font; int font_size = 32;
			/* The default scale of the characters;
			 * Single chars can modify this value through
			 * the size_mod value in their ObjectRepresentation
			 */
			float standard_scale = 1;
			/* Fill the background of the frame;
			 * Useful for status bars and stuff
			 * that should not be obstructed by other frames
			 */
			sf::Color frame_bg_col = sf::Color(0, 0, 0, 0);
			/* Aligns the frame_bg to the last character that is
			 * still rendered because it's almost 100% not even with
			 * the end position*/
			bool fit_to_text = true;
			/* Top left beginning of text */
			sf::Vector2f origin;
			/* Bottom right end */
			sf::Vector2f end;
			
			/* Fills the grid up with default grid objects 
			 * This is used to ensure that the grid always corresponds
			 * to it's wanted size 
			 */
			void fill_grid();
	};
};
