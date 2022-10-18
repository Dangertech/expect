#pragma once
 
#include "frama.hpp"
#include "ecs.hpp"
#include "fabric.hpp"
#include "util.hpp"
#include "cli.hpp"

namespace in
{
	/* Contains static data and functions for showing things to the gameview */
	class RepCreator
	{
		public:
			RepCreator(ecs::Aggregate* magg)
			{
				agg = magg;
				load_textures();
			}
			struct Rep
			{
				/* 0 means chrrep should be used, 1 means imgrep
				 * should be used 
				 */
				bool to_use = 0; 
				fr::ChrRep chrrep;
				fr::ImgRep imgrep;
			};
			/* Main function of this class;
			 * Evaluates a character it is sent and returns
			 * a struct where either a procedurally generated ChrRep or 
			 * an ImgRep should be drawn to a gameview frame
			 */
			Rep evaluate_rep(ecs::entity_id);
		private:
			ecs::Aggregate* agg;
			fr::ChrRep evaluate_chr(ecs::entity_id);
			fr::ImgRep evaluate_img(ecs::entity_id);
			 
			sf::Texture tilemap;
			 
			enum Tilename 
			{ 
				FOOD_SLIME_MOLD,
				FOOD_RATION
			};
			const int tile_x = 32, tile_y = 32;
			const std::unordered_map<Tilename, Vec2> tilelocs
			{
				{FOOD_SLIME_MOLD, {0, 0}}, 
				{FOOD_RATION, {1, 0}} 
			};
			/* Looks up a tile location from tilelocs and
			 * converts it to an IntRect
			 */
			sf::IntRect tloc(Tilename);
			 
			void load_textures();
	};
	 
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
			RepCreator* rc;
			fr::Frame* cli_frame;
			
			int extra_gv_layers = 1;
			/* Sets up the frames in the gv vector according to extra_gv_layers */
			void make_layers();
			
			bool use_shaders = true;
			
			ecs::Aggregate* agg;
			cli::CliData* cli_dat;
			Vec3 cam_center;
			
			void update_sizes();
			/* Logic to fill the gameview and cli frames with appropriate 
			 * characters
			 */
			void fill_gv(fr::Frame&, int z, bool below = false, 
					float transparency = 255.f);
			void fill_cli();
			
			double seconds_since_startup = 0;
			
			struct CamView
			{
				Vec2 xbounds, ybounds;
				int z;
				std::vector<ecs::entity_id> entts;
				 
				CamView(ecs::Aggregate* agg, Vec2 mxb, Vec2 myb, int mz);
			};
			Vec2 eval_position(fa::Position& pos, sf::Vector2i gvsize, int z);
			/* Holds entity ids corresponding to a specific z layer in the 
			 * world; Update mechanic is pretty complicated, so the layers are
			 * simply cleared every time the player presses a button in get_events();
			 * CamViews are created dynamically on fill_gv()
			 */
			std::unordered_map<int, CamView*> cvs;
	};
	 
	/* Returns true for seconds_on seconds and then
	 * false for seconds_off seconds based on the system
	 * time and how many milliseconds have passed since
	 * the epoch timestamp; Primarily used for timing
	 * animations
	 */
	bool anim_is_active(float seconds_on, float seconds_off);
}
