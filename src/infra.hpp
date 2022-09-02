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
			
			void adjust_zoom(float chg);
			Vec2 get_cam_center() {return cam_center;}
			void set_cam_center(Vec2 c) {cam_center = c;}
		private:
			sf::RenderWindow* win;
			/* Font used in the game view */
			sf::Font* gv_font;
			/* Font used for readable text */
			sf::Font* tx_font;
			sf::Shader* bloom;
			fr::Frame* gv;
			fr::Frame* cli_frame;
			
			ecs::Aggregate* agg;
			cli::CliData* cli_dat;
			Vec2 cam_center;
			
			void update_sizes();
			/* Draw CliData to a chosen frame */
			void draw_cli(fr::Frame&, cli::CliData&);
			
			void border(fr::Frame&, sf::Color);
			/* Converts a high-level drawable struct
			 * to the frama-specific ObjectRepresentations
			 */
			fr::ObjRep drw_to_objrep(fa::Drawable drw);
	};
}
