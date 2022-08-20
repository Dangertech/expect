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
			/*
			 * clears the gameview grid and then draws
			 * every visible object back into the gameview
			 * frame; Setting force to true implies that the
			 * connected aggregate has updated its state, otherwise
			 * the frame will only be updated if the GfxManager
			 * has a reason by itself to do so (window resized,
			 * redraw to avoid missing characters, etc.);
			 * Returns true if it actually updated;
			 */
			bool render_gv(bool force = false);
			/* Actually draws the objects to the 
			 * RenderWindow which then displays them
			 */
			bool display_frames();
			
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
			cli::CliGraphics* cli_graphics;
			
			ecs::Aggregate* agg;
			cli::CliData* cli_dat;
			Vec2 cam_center;
			bool queue_render = false;
			
			void update_sizes();
			
			/* Converts a high-level drawable struct
			 * to the frama-specific ObjectRepresentations
			 */
			fr::ObjRep drw_to_objrep(fa::Drawable drw);
	};
}
