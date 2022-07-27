#pragma once
 
#include "frama.hpp"
#include "ecs.hpp"
#include "fabric.hpp"

namespace in
{
	/* Manages game view, sidebars and other windows */
	class GfxManager
	{
		public:
			GfxManager();
			void adjust_zoom(float chg);
			void render_gv(int ctr_x, int ctr_y, ecs::Aggregate* agg);
			bool win_open() {return win->isOpen();}
			std::vector<sf::Event> get_events();
			bool display_frames();
		private:
			sf::RenderWindow* win;
			sf::Font* font;
			fr::Frame* gv;
			fr::Frame* sb;
			
			void update_sizes();
	};
}
