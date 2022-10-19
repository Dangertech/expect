#include "infra.hpp"
#include "frama.hpp"
#include "const.h"
#include "settings.hpp"
#include <iostream>
#include <unistd.h>
#include <chrono>


in::GfxManager::GfxManager(ecs::Aggregate& my_agg, cli::CliData& my_cli_dat)
{
	SettingContainer s;
	/* I'm using pointers because I couldn't figure out
	 * how to initialize objects directly in a constructor
	 */
	agg = &my_agg;
	cli_dat = &my_cli_dat;
	win = new sf::RenderWindow(sf::VideoMode(1920, 1080), "expect");
	gv_font = new sf::Font;
	tx_font = new sf::Font;
	blur = new sf::Shader;
	rc = new RepCreator(agg);
	if (!sf::Shader::isAvailable())
		use_shaders = false;
	if (!blur->loadFromFile("blur.frag", sf::Shader::Fragment))
	{
		std::cout << "Graphics Initialization Error: Blur shader could "
			<< "not be initialized!" << std::endl;
		use_shaders = false;
	}
	gv_font->loadFromFile(s.get_gv_font_name());
	tx_font->loadFromFile(s.get_tx_font_name());
	/* Create gv layers based on extra_gv_layers */
	make_layers();
	cli_frame = new fr::Frame(*tx_font, s.get_tx_font_size(), 
			sf::Vector2i(0, 0), sf::Vector2i(5, 5));
	/* Setting this doesn't matter:
	 * When win.display() is called, sfml just waits for the time remaining
	 * to fulfill the fps requirement;
	 * Instead, the fps are defined in settings.hpp
	 * and executed in the gfx::delay(), called in main()
	 */
	//win->setFramerateLimit(30);
	gv[0]->set_frame_bg(sf::Color::Black);
	for (auto x : gv)
		x->set_margin(sf::Vector2i(2,2));
	cli_frame->set_frame_bg(sf::Color::Black);
	cli_frame->set_standard_scale(0.6f);
	update_sizes();
}
in::GfxManager::~GfxManager()
{
	delete win;
	delete blur;
	delete gv_font;
	delete tx_font;
	for (auto x : gv)
		delete x;
	delete cli_frame;
	delete rc;
}

void in::GfxManager::adjust_zoom(float chg)
{
	try
	{
		for (auto x : gv)
			x->set_standard_scale(x->get_standard_scale()+chg);
	}
	catch (int e)
	{
		if (e == ERR_OVERFLOW)
			std::cout << "Maximum zoom level reached!" << std::endl;
	}
	update_sizes();
}

std::vector<sf::Event> in::GfxManager::get_events()
{
	std::vector<sf::Event> ret;
	sf::Event e;
	while(win->pollEvent(e))
	{
		if (e.type == sf::Event::Closed)
			win->close();
		if (e.type == sf::Event::Resized)
		{
			win->setView(sf::View(sf::FloatRect(0,0,e.size.width, e.size.height)));
			update_sizes();
		}
		/* Push back into a simulated queue to be batch
		 * processed externally
		 */
		ret.push_back(e);
	}
	
	return ret;
}

sf::RenderTexture* create_tex(sf::RenderWindow* win)
{
	sf::RenderTexture* tex = new sf::RenderTexture;
	if (!tex->create(win->getSize().x, win->getSize().y))
	{
		std::cout 
			<< "What the fuck is happening? RenderTexture creation failed!" << std::endl;
		throw ERR;
	}
	tex->clear(sf::Color::Black);
	return tex;
}

void in::GfxManager::render()
{
	win->clear();
	/* Gameview drawing */
	for (int i = 0; i<gv.size(); i++)
	{
		gv[i]->clear();
		float lalph = i>0 ? 128.f : 255.f;
		fill_gv(*gv[i], cam_center.z+i, i > 0 ? false : true, lalph);
		fr::anim::slide_down(seconds_since_startup*0.8, 
				*gv[i], 5, sf::Color::Green, true, i > 0 ? true : false);
	}
	fr::anim::border(*gv[0], 
			cli_dat->get_active() ? sf::Color(128,128,128) : sf::Color(CLI_ACTIVE), 
			0x2014, 0x2014, L'|', L'|', L'/', L'|', L' ', 0x2014);
	
	/* CLI drawing */
	cli_frame->clear();
	fill_cli();
	fr::anim::slide_down(seconds_since_startup*0.8, 
			*cli_frame, 5, sf::Color::Green, false);
	fr::anim::border(*cli_frame, 
			cli_dat->get_active() ? sf::Color(CLI_ACTIVE) : sf::Color(128, 128, 128));
	 
	
	/* Execute the draw functions to inform the window of the
	 * sprites in the frama frame; Run them
	 * with a RenderTexture to do compositing
	 */
	if (use_shaders)
	{
		/* Pass shader parameters */
		blur->setUniform("tex", sf::Shader::CurrentTexture);
		blur->setUniform("strength", 5.f);
		blur->setUniform("separation", 3.f);
		/* Set up render texture */
		sf::RenderTexture* initex = create_tex(win);
		sf::RenderTexture* blurtex = create_tex(win);
		/* Initial Drawing */
		for (auto x : gv)
			x->draw(initex);
		cli_frame->draw(initex);
		initex->display();
		/* Shader passes */
		sf::RenderStates state;
		state.blendMode = sf::BlendAdd;
		blurtex->draw(sf::Sprite(initex->getTexture()), blur);
		blurtex->draw(sf::Sprite(initex->getTexture()), state);
		blurtex->display();
		 
		/* Draw the final RenderTexture to the window */
		sf::Sprite intersprite(blurtex->getTexture());
		win->draw(intersprite);
		delete initex;
		delete blurtex;
	}
	else
	{
		/* Use direct rendering to the window */
		for (auto x : gv)
			x->draw(win);
		cli_frame->draw(win);
	}
	/* Display the window */
	win->display();
}

void in::GfxManager::delay(double time_spent)
{
	/*
	std::cout << 1.0/time_spent << " FPS (theoretical)" << std::endl; 
	*/
	SettingContainer set;
	double goal_time = 1.0/set.get_fps()-time_spent;
	seconds_since_startup += time_spent + goal_time;
	if (goal_time > 0.0)
	{
		/*
		std::cout << "Waiting for " << goal_time << " seconds!" << std::endl;
		*/
		usleep(goal_time*1000000);
	}
	else
		std::cout << "Hanging behind!" << std::endl;
}

void in::GfxManager::update_sizes()
{
	sf::Vector2u size = win->getSize();
	int margin = smaller(size.x, size.y)*0.02;
	SettingContainer s;
	int gv_w = size.x-s.get_sidebar_width()-margin*2; /* gameview width */
	if (gv_w < 0)
		gv_w = 0;
	 
	try
	{
		for (auto x : gv)
		{
			x->set_origin(sf::Vector2i(margin, margin));
			x->set_end(sf::Vector2i(gv_w, size.y-margin));
		}
		if (cli_frame)
		{
			cli_frame->set_origin(sf::Vector2i(gv_w, margin));
			cli_frame->set_end(sf::Vector2i(size.x-margin, size.y-margin));
		}
	}
	catch(int e)
	{
		if (e == ERR)
		{
			/* Coming soon: A logger */
			std::cout << "Origin smaller than end" << std::endl;
		}
		
	}
	/* Refresh camera views to incorporate new objects that might
	 * have appeared in view
	 */
	cvs.clear(); 
}

Vec2 in::GfxManager::eval_position(fa::Position& pos, sf::Vector2i gvsize, int z)
{
	if (pos.z != z)
		return {-1, -1};
	int x = pos.x - cam_center.x + gvsize.x/2, 
			y = pos.y - cam_center.y + gvsize.y/2;
	if (x >= 0 && x < gvsize.x && y >= 0 && y < gvsize.y)
		return {x,y};
	return {-1,-1};
}
 
void in::GfxManager::fill_gv(fr::Frame& f, int z, bool below, float transparency)
{
	sf::Vector2i gvsize = f.get_grid_size();
	Vec2 xbounds = {cam_center.x-gvsize.x/2, cam_center.x+gvsize.x/2};
	Vec2 ybounds = {cam_center.y-gvsize.y/2, cam_center.y+gvsize.y/2};
	
	 
	if (below)
	{
		/* View for the below render */
		CamView* cblw;
		if (cvs.find(z-1) == cvs.end())
		{
			cvs[z-1] = new CamView(agg, xbounds, ybounds, z-1);
			cblw = cvs[z-1];
		}
		else
			cblw = cvs[z-1];
		/* This loop draws everything one unit below the player (the floor)
		 */
		for (ecs::entity_id ent : cblw->entts)
		{
			fa::Position* pos = agg->get_cmp<fa::Position>(ent);
			int x = pos->x - cam_center.x + gvsize.x/2, 
					y = pos->y - cam_center.y + gvsize.y/2;
			if (x < 0 || y < 0 || x >gvsize.x || y > gvsize.y)
				continue;
			/* Draw it with less alpha */
			fr::MultiRep rep = rc->evaluate_rep(ent);
			if (rep.type == fr::CHR) 
			{
				rep.chrrep.fill.a -= 130;
				rep.chrrep.bg.a -= 130;
				rep.chrrep.ch = L'.';
				f.set_char(rep.chrrep, x, y);
			}
			else 
			{
				rep.imgrep.col.a -= 130;
				f.set_char(rep.imgrep, x, y);
			}
		}
	}
	 
	/* View for the standard render */
	CamView* cstd;
	if (cvs.find(z) == cvs.end())
	{
		/* Create a new one if a camview for this z axis doesn't exist yet */
		cvs[z] = new CamView(agg, xbounds, ybounds ,z);
		cstd = cvs[z];
	}
	else 
		cstd = cvs[z];
	/* This loop draws everything on the same level as the player
	 */
	for (ecs::entity_id ent : cstd->entts)
	{
		if (!agg->get_cmp<fa::Position>(ent))
			continue;
		Vec2 phpos = eval_position(*agg->get_cmp<fa::Position>(ent), gvsize, z);
		if (phpos.x != -1)
		{
			fr::MultiRep rep = rc->evaluate_rep(ent);
			if (rep.type == fr::CHR)
			{
				rep.chrrep.fill.a = transparency;
				rep.chrrep.bg.a = transparency;
				f.set_char(rep.chrrep, phpos.x, phpos.y);
			}
			else
			{
				f.set_char(rep.imgrep, phpos.x, phpos.y);
			}
		}
	}
	
	/* For now, the function just rerenders everything that should be
	 * displayed above everything else (everything that is alive and
	 * player characters)
	 */
	for (ecs::entity_id ent : cstd->entts) 
	{
		if (!agg->get_cmp<fa::Playable>(ent) || !agg->get_cmp<fa::Alive>(ent)
				|| !agg->get_cmp<fa::Position>(ent))
			continue;
		Vec2 phpos = eval_position(*agg->get_cmp<fa::Position>(ent), gvsize, z);
		if (phpos.x != -1)
		{
			fr::MultiRep rep = rc->evaluate_rep(ent);
			if (rep.type == fr::CHR)
			{
				f.set_char(rep.chrrep, phpos.x, phpos.y);
			}
			else
			{
				f.set_char(rep.imgrep, phpos.x, phpos.y);
			}
		}
	}
}

void in::GfxManager::fill_cli()
{
	int num_entries = cli_dat->num_entries();
	int size_y = cli_frame->get_grid_size().y;
	int size_x = cli_frame->get_grid_size().x;
	int bottom_margin = 3;
	/* Dry test to determine how many lines are needed extra for
	 * multiline entries
	 */
	int extra = 0;
	for (int i = 0; i<size_y - bottom_margin; i++)
	{
		cli::LogEntry this_entry(L"", cli::DEBUG);
			try
			{
				this_entry = cli_dat->get_entry(num_entries+i-size_y+bottom_margin);
			}
			catch (int e)
			{
			}
			sf::Vector2i c = cli_frame->print(this_entry.c, 4, i, fr::EMPTY_CHR, true, 
					size_x-2, -1, true);
			if (c.y != i)
				extra += c.y-i;
	}
	 
	/* Location of the current entry in the vector */
	int eloc = num_entries-size_y+bottom_margin+extra;
	for (int i = 0; i<size_y-bottom_margin; i++)
	{
		cli::LogEntry this_entry(L"", cli::DEBUG);
		try
		{
			this_entry = cli_dat->get_entry(eloc);
		}
		catch (int e)
		{
		}
		std::wstring msg;
		int x = 4;
		fr::ChrRep rep = fr::EMPTY_CHR;
		switch (this_entry.l)
		{
			case cli::INPUT:
				msg += L"> "; 
				x = 2;
				rep.bold = true;
				rep.fill = sf::Color(CLI_USER);
				break;
			case cli::DEBUG:
				rep = fr::ChrRep(L' ', sf::Color(128, 128, 128));
				break;
			case cli::MESSAGE:
				break;
		}
		msg += this_entry.c;
		sf::Vector2i c(x,i);
		if (msg.size())
		{
			/* Now print for real */
			c = cli_frame->print(msg, x, i, rep, true, size_x-2, -1, false);
		}
		i = c.y;
		eloc += 1;
	}
	 
	/* Draw the "PS1" of the CLI input */
	fr::ChrRep s(CLI_PS1);
	if (cli_dat->get_active())
	{
		s.fill = sf::Color(CLI_ACTIVE);
		s.bg = sf::Color(CLI_ACTIVE, CLI_ALPHA);
		/* Box */
		for (int y = size_y-bottom_margin; y<size_y-1; y++)
		{
			for(int x = 2; x<size_x-2; x++)
			{
				cli_frame->set_char(fr::ChrRep(L' ', sf::Color::Black, 
							sf::Color(CLI_ACTIVE, CLI_ALPHA)), 
						x, y);
			}
		}
	}
	/* PS1 */
	cli_frame->set_char(s, 2, size_y-bottom_margin);
	if (cli_dat->get_active())
	{
		/* Input Buffer (Active) */
		sf::Vector2i endpos = cli_frame->print(cli_dat->get_bfr(), 4, 
				size_y-bottom_margin, {L' ', sf::Color(CLI_USER), 
				sf::Color(CLI_ACTIVE, CLI_ALPHA)},
				true, size_x-3);
		/* "Cursor" */
		if (in::anim_is_active(1.0, 0.5))
		{
			cli_frame->set_char({L'|', sf::Color::White, 
					sf::Color(CLI_ACTIVE, CLI_ALPHA)}, endpos.x, endpos.y);
		}
	}
	else
	{
		cli_frame->print(cli_dat->get_bfr(), 4, 
				size_y-bottom_margin, {L' ', sf::Color(CLI_USER), 
				sf::Color(0,0,0, CLI_ALPHA)});
	}
}

void in::GfxManager::set_extra_layers(int l)
{
	if (l < 0)
		throw ERR;
	if (l > 7)
		throw ERR_OVERFLOW;
	extra_gv_layers = l;
	make_layers();
}

void in::GfxManager::make_layers()
{
	SettingContainer s;
	gv.clear(); /* Wipe all layers to safely start afresh */
	while (extra_gv_layers+1 > gv.size())
	{
		gv.push_back(new fr::Frame(*gv_font, s.get_gv_font_size(), 
				sf::Vector2i(0, 0), sf::Vector2i(5, 5)));
		gv[gv.size()-1]->set_standard_scale(1+(0.1*(gv.size()-1)));
	}
	update_sizes();
	std::wcout << gv.size() << std::endl;
}

in::GfxManager::CamView::CamView(ecs::Aggregate* agg, Vec2 mxb, Vec2 myb, int mz)
{
	if (mxb.x > mxb.y)
		throw ERR;
	if (myb.x > myb.y)
		throw ERR;
	xbounds = mxb;
	ybounds = myb;
	z = mz;
	for (ecs::entity_id ent : ecs::AggView<fa::Position>(*agg))
	{
		fa::Position* pos = agg->get_cmp<fa::Position>(ent);
		if (pos->z != z)
			continue;
		if (pos->x < xbounds.x || pos->x > xbounds.y)
			continue;
		if (pos->y < ybounds.x || pos->y > ybounds.y)
			continue;
		entts.push_back(ent);
	}
}

bool in::anim_is_active(float seconds_on, float seconds_off)
{
	int ms_on = seconds_on*1000; int ms_off = seconds_off*1000;
	using namespace std::chrono;
	high_resolution_clock::time_point now = high_resolution_clock::now();
	unsigned long long ms_since_epoch = 
		duration_cast<milliseconds>(now.time_since_epoch()).count();
	if (ms_since_epoch%(ms_on+ms_off) < ms_on)
	{
		return true;
	}
	return false;
}

namespace in
{
	fr::MultiRep RepCreator::evaluate_rep(ecs::entity_id id)
	{
		fr::ChrRep crep = evaluate_chr(id);
		fr::ImgRep irep = evaluate_img(id);
		if (irep.area != sf::IntRect(0,0,0,0))
			return {fr::IMG, crep, irep};
		else
			return {fr::CHR, crep, irep};
	}
	
	fr::ChrRep RepCreator::evaluate_chr(ecs::entity_id id)
	{
		/* The task of this function might grow incredibly complex
		 * and is already moderately complex; It looks a bit hacked together
		 * right now, but that's OK.
		 */
		fr::ChrRep crep(L'?');
		fr::ImgRep irep;
		if(agg->get_cmp<fa::Playable>(id))
		{
			crep.ch = L'@';
		}
		else if (agg->get_cmp<fa::Wall>(id))
		{
			fa::Wall* w = agg->get_cmp<fa::Wall>(id);
			crep.ch = L'#';
			switch (w->type)
			{
				case fa::Wall::CONCRETE:
					crep.fill = sf::Color(128,128,128);
					break;
				case fa::Wall::REDBRICK:
					crep.fill = sf::Color(255, 80, 0);
					break;
			}
			fa::Paintable* p = agg->get_cmp<fa::Paintable>(id);
			if (p)
			{
				/* TODO: Merge the base color and the paintable color based on
				 * the visibility
				 */
				if (p->visibility > 0)
				{
					crep.fill.r = (crep.fill.r + p->color.x)/2;
					crep.fill.g = (crep.fill.g + p->color.y)/2;
					crep.fill.b = (crep.fill.b + p->color.z)/2;
				}
			}
		}
		else if (agg->get_cmp<fa::Eatable>(id))
		{
			fa::Eatable* e = agg->get_cmp<fa::Eatable>(id);
			switch (e->type)
			{
				case fa::Eatable::RATION:
					crep.ch = L'&';
					crep.fill = sf::Color(98,54,18);
					break;
				case fa::Eatable::SLIME_MOLD:
					crep.ch = L'o';
					crep.fill = sf::Color(15, 168, 142);
			}
		}
		
		
		if (agg->get_cmp<fa::Flammable>(id))
		{
			if (agg->get_cmp<fa::Flammable>(id)->burning)
			{
				if (in::anim_is_active(0.5, 1.f))
				{
					crep.bg = sf::Color(255, 128, 0);
				}
			}
		}
		return crep;
	}
	
	fr::ImgRep RepCreator::evaluate_img(ecs::entity_id id)
	{
		fr::ImgRep irep;
		irep.txt = nullptr;
		if (tilemap.getSize() == sf::Vector2u(0,0))
		{
			/* Just chicken out and always use characters */
			std::cout << "Size is 0" << std::endl;
			return irep;
		}
		irep.txt = &tilemap;
		if (agg->get_cmp<fa::Eatable>(id))
		{
			fa::Eatable* e = agg->get_cmp<fa::Eatable>(id);
			switch (e->type)
			{
				case fa::Eatable::SLIME_MOLD:
					irep.area = tloc(FOOD_SLIME_MOLD);
					break;
				case fa::Eatable::RATION:
					irep.area = tloc(FOOD_RATION);
				default:
					break;
			}
		}
		return irep;
	}
	
	void RepCreator::load_textures()
	{
		if (!tilemap.loadFromFile("art/tilemap.png"))
		{
			std::cout << "Error! Couldn't load tilemap from ./art/tilemap.jpg!" 
				<< std::endl;
		} 
	}

	sf::IntRect RepCreator::tloc(Tilename t)
	{
		if (tilelocs.find(t) == tilelocs.end())
			return sf::IntRect(0,0,0,0);
		Vec2 metapos = tilelocs.at(t);
		return sf::IntRect(metapos.x*tile_x, metapos.y*tile_y, tile_x, tile_y);
	}
}
