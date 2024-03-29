#pragma once
 
#include "frama.hpp"
#include "ecs.hpp"
#include "fabric.hpp"
#include "util.hpp"
#include "cli.hpp"

namespace in
{
	/* Manages game view, sidebars and other windows */
	class GfxManager
	{
		public:
			GfxManager(ecs::Aggregate&, cli::CliData&);
			~GfxManager();
			bool win_open() {return win->isOpen();}
			std::vector<sf::Event> get_events();
			/* Draw the stuff in view to the gameview frame,
			 * Draw the stuff of the cli to the cli frame,
			 * execute the frame::draw() functions,
			 * display the window
			 */
			void render();
			/* Wait some amount of time to meet the fps requirement;
			 * To be supplied is the time taken between the beginning of
			 * the game loop and it's end 
			 */
			void delay(double time_spent);
			
			void adjust_zoom(float chg);
			Vec2 get_cam_center() {return cam_center;}
			void set_cam_center(Vec2 c) {cam_center = c;}
		private:
			sf::RenderWindow* win;
			/* Font used in the game view */
			sf::Font* gv_font;
			/* Font used for readable text */
			sf::Font* tx_font;
			sf::Shader* blur;
			fr::Frame* gv;
			fr::Frame* cli_frame;
			
			bool use_shaders = true;
			
			ecs::Aggregate* agg;
			cli::CliData* cli_dat;
			Vec2 cam_center;
			
			void update_sizes();
			/* Logic to fill the gameview and cli frames with appropriate characters*/
			void fill_gv();
			void fill_cli();
			
			/* Converts a high-level drawable struct
			 * to the frama-specific ObjectRepresentations
			 */
			fr::ObjRep drw_to_objrep(fa::Drawable drw);
			double seconds_since_startup = 0;
			
			/* Returns true for seconds_on seconds and then
			 * false for seconds_off seconds based on the system
			 * time and how many milliseconds have passed since
			 * the epoch timestamp; Primarily used for timing
			 * animations
			 */
			bool anim_is_active(float seconds_on, float seconds_off);
	};
}
