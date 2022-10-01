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
			Vec3 get_cam_center() {return cam_center;}
			void set_cam_center(Vec3 c) {cam_center = c;}
			void set_shaders(bool s) {use_shaders = s;}
			bool get_shaders() {return use_shaders;}
			void set_extra_layers(int l);
		private:
			sf::RenderWindow* win;
			/* Font used in the game view */
			sf::Font* gv_font;
			/* Font used for readable text */
			sf::Font* tx_font;
			sf::Shader* blur;
			std::vector<fr::Frame*> gv;
			fr::Frame* cli_frame;
			
			int extra_gv_layers = 2;
			void make_layers();
			
			bool use_shaders = true;
			
			ecs::Aggregate* agg;
			cli::CliData* cli_dat;
			Vec3 cam_center;
			
			void update_sizes();
			/* Logic to fill the gameview and cli frames with appropriate characters*/
			void fill_gv(fr::Frame&, int z, bool below = false, float transparency = 255.f);
			void fill_cli();
			Vec2 eval_position(fa::Position& pos, sf::Vector2i gvsize, int z);
			
			double seconds_since_startup = 0;
	};
	 
	/* Returns true for seconds_on seconds and then
	 * false for seconds_off seconds based on the system
	 * time and how many milliseconds have passed since
	 * the epoch timestamp; Primarily used for timing
	 * animations
	 */
	bool anim_is_active(float seconds_on, float seconds_off);
	
	/* Contains static data and functions for showing things to the gameview */
	namespace gv
	{
		fr::ObjRep evaluate_rep(ecs::Aggregate*, ecs::entity_id);
	}
}
