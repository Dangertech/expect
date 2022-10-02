// Library to manage *everything* low-level rendering related
/* - At it's core it should just provide a way to render grids filled
 *   with colored letters through sfml;
 * - Should be able to be used standalone for other projects
 */
#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include "const.h"

namespace fr
{
	enum CharPoint {TOP, LEFT, RIGHT, DOWN, TOP_LEFT, TOP_RIGHT, 
		BOTTOM_LEFT, BOTTOM_RIGHT, CENTER};
	 
	struct ChrRep
	{
		wchar_t ch;
		float size_mod;
		sf::Color fill;
		/*
		sf::Color ol; // outline
		unsigned int ol_thickness;
		*/
		sf::Color bg;
		bool bold;
		 
		ChrRep
		(
			wchar_t my_ch,
			sf::Color my_fill = sf::Color(255, 255, 255, 255),
			sf::Color my_bg = sf::Color(0, 0, 0, 0), 
			float my_size_mod = 1, 
			bool my_bold = false
			/*
			sf::Color my_ol = sf::Color(0, 0, 0, 0), 
			unsigned my_ol_thickness = 0,
			*/
		)
		{
			ch = my_ch;
			size_mod = my_size_mod; fill = my_fill;
			/*
			ol = my_ol; ol_thickness = my_ol_thickness;
			*/
			bg = my_bg; bold = my_bold;
		}
		ChrRep(){}
		 
		bool operator==(const ChrRep& r) const
		{
			return ch == r.ch && size_mod == r.size_mod
				&& fill == r.fill /*&& ol == r.ol
				&& ol_thickness == r.ol_thickness*/
				&& bg == r.bg && bold == r.bold;
		}
	};
	const ChrRep EMPTY(L' ');

	struct ImgRep
	{
		sf::Texture* txt;
		sf::IntRect area;
		sf::Color col;
		sf::Color bg;
		int size_mod = 1;
		ImgRep
		(
			sf::Texture* mtxt,
			sf::IntRect marea = sf::IntRect(0,0,0,0), /* Whole texture used */
			sf::Color mcol = sf::Color(0,0,0,0),
			sf::Color mbg = sf::Color(0,0,0,0),
			int msize_mod = 1
		)
		{
			txt = mtxt; area = marea; col = mcol; bg = mbg; size_mod = msize_mod;
		}
		ImgRep(){}
	};
	 
	class Frame
	{
		public:
			Frame(sf::Font& my_font, 
				int my_font_size,
				sf::Vector2i my_ori, 
				sf::Vector2i my_end)
			{
				font = &my_font;
				font_size = my_font_size;
				origin = my_ori;
				end = my_end;
				
				font_txt = &font->getTexture(font_size);
				if (get_char_size().x == 0 || get_char_size().y == 0)
					throw ERR_INVALID_IPT;
				 
				/* Add a local bound rectangle that can be
				 * referenced to get the size of any character
				 */
				set_size_ref();
				reserve_grid(); 
				grid = {};
			}
			 
			/* Get the object representation of a character back;
			 * Please note that this lookup is very expensive as all
			 * objects are stored in an unsorted vector to make performance
			 * improvements in other areas, so it's best to use it sparingly
			 */
			ChrRep get_char(int x, int y);
			/* Set a character on the grid by providing a specification
			 * in form of a ChrRep or ImgRep
			 */
			void set_char(ChrRep rep, int x, int y);
			void set_char(ImgRep rep, int x, int y);
			
			/* Allows to print a whole string in a line
			 * at a specified position. 
			 * In the given ChrRep, the character is ignored but
			 * all characters will obey to the specified styling;
			 * Newlines are supported and will return to the
			 * specified x position one row below.
			 * - If autobreak is true, line breaks will be made automatically
			 * if the function runs out of space in a line. If it is false,
			 * the function will simply truncate until a manual \n is encountered;
			 * - max_x and max_y determine the maximum location until which is
			 * printed (in case of autobreak, a linebreak is made when max_x is
			 * reached);
			 * - If 'dry' is set to true, the function will not print anything but
			 * still return the theoretical line changes
			 * Return:
			 *  Returns the "cursor" position upon completion;
			 *  If the function ran out of y space, it returns the latest y coordinate
			 */
			sf::Vector2i print(std::wstring input, int x, int y, 
					ChrRep rep, bool autobreak = true, int max_x = -1, int max_y = -1, 
					bool dry = false);
			
			float get_standard_scale() {return standard_scale; }
			void set_standard_scale(float scale);
			 
			sf::Color get_frame_bg();
			void set_frame_bg(sf::Color col) {frame_bg_col = col;}
			
			sf::Vector2i get_origin() { return origin; }
			void set_origin(sf::Vector2i origin);
			
			sf::Vector2i get_end() { return end; }
			void set_end(sf::Vector2i end);
			
			sf::Vector2i get_margin() { return margin; }
			void set_margin(sf::Vector2i m) {margin = m;}
			
			bool get_fit_to_text() { return fit_to_text; }
			void set_fit_to_text(bool new_state) {fit_to_text = new_state;}
			
			bool get_end_before_end() { return end_before_end; }
			void set_end_before_end(bool new_state) {end_before_end = new_state;}
			
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
			 * at any of the 9 geometrical points 
			 */
			sf::Vector2f get_char_pos(int x, int y, CharPoint point = CENTER);
			
			/* Takes in a window coordinate and returns the
			 * grid coordinate of this frame. If there is no
			 * grid at this position, it throws ERR
			 */
			sf::Vector2<int> get_char_at(int win_x, int win_y);
			
			/* Pushes the changes to the connected window
			 * to draw them
			 * Returns 1 if new changes have been pushed,
			 * Returns 0 if no new changes were detected and nothing was changed
			 * Throws ERR if the internal grid is empty (impossible by design)
			 */
			int draw(sf::RenderTarget* win, sf::Shader* shad = nullptr);
			void clear(); /* Empties the grid */
			void force_update(); /* Force an update on the next draw() */
			bool get_update(); /* Get if the frame will update on the next draw() */
		private:
			bool to_update = false;
			struct GridObj
			{
				sf::Sprite s;
				sf::RectangleShape r;
				float size_mod = 1;
				ChrRep refchr;
				ImgRep refimg;
				bool uses_img = false;
			};
			std::unordered_map<unsigned long, GridObj> grid;
			sf::Font* font; int font_size = 32;
			const sf::Texture* font_txt;
			sf::RectangleShape frame_bg;
			/* Keeps track of the size of a character through
			 * set_size_ref and can be referenced by all functions;
			 * Returned to the user through get_standard_char_size
			 */
			sf::Rect<float> sref;
			/* The default scale of the characters;
			 * Single chars can modify this value through
			 * the size_mod value in their ObjectRepresentation
			 */
			float standard_scale = 1;
			/* Additional margin between characters in the grid (in pixels) */
			sf::Vector2i margin = sf::Vector2i(0,0);
			/* Fill the background of the frame;
			 * Useful for status bars and stuff
			 * that should not be obstructed by other frames
			 */
			sf::Color frame_bg_col = sf::Color(0, 0, 0, 0);
			/* Aligns the frame_bg to the last character that is
			 * still rendered because it's almost 100% not even with
			 * the end position
			 */
			bool fit_to_text = true;
			/* Determines if the final row/column of characters is
			 * before or after the endpoint; By default, it's after
			 * the set end point
			 */
			bool end_before_end = false;
			/* Top left beginning of text */
			sf::Vector2i origin;
			/* Bottom right end */
			sf::Vector2i end;
			
			/* Reserves new space on the grid (std::vector::reserve())
			 * according to the get_grid_size() function
			 */
			void reserve_grid();
		 /* Sets the sref sf::Rect to reference
			* the size of a character; This is done on
			* object initialization
			*/
			void set_size_ref();
	};
	 
	 
	/* Execute various animations (and other effects) on a frame;
	 * Supply t as a number between 0 or 1 that determines how far the animation
	 * has progressed
	 */
	namespace anim
	{
		void slide_down(float t, Frame& fr, int slices, sf::Color slice_color, bool reverse = false, bool only_filled = false); 
		void border(fr::Frame&, sf::Color, wchar_t top = 0x2550,
			wchar_t bottom = 0x2550, wchar_t left = 0x2551, wchar_t right = 0x2551, 
			wchar_t top_left = 0x2554, 
			wchar_t top_right = 0x2557, wchar_t bottom_right = 0x255d,
			wchar_t bottom_left = 0x255a);
	}
};
