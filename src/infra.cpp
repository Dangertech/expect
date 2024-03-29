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
	gv = new fr::Frame(*gv_font, s.get_gv_font_size(), 
			sf::Vector2i(0, 0), sf::Vector2i(5, 5));
	cli_frame = new fr::Frame(*tx_font, s.get_tx_font_size(), 
			sf::Vector2i(0, 0), sf::Vector2i(5, 5));
	/* Setting this doesn't matter:
	 * When win.display() is called, sfml just waits for the time remaining
	 * to fulfill the fps requirement;
	 * Instead, the fps are defined in settings.hpp
	 * and executed in the gfx::delay(), called in main()
	 */
	//win->setFramerateLimit(30);
	gv->set_frame_bg(sf::Color::Black);
	gv->set_margin(sf::Vector2i(2,2));
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
	delete gv;
	delete cli_frame;
}

void in::GfxManager::adjust_zoom(float chg)
{
	try
	{
		gv->set_standard_scale(gv->get_standard_scale()+chg);
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
	
	 
	/* Gameview drawing */
	gv->clear();
	fill_gv();
	fr::anim::slide_down(seconds_since_startup*0.8, *gv, 5, sf::Color::Green, true);
	fr::anim::border(*gv, 
			cli_dat->get_active() ? sf::Color(128,128,128) : sf::Color(CLI_ACTIVE), 
			0x2014, 0x2014, L'|', L'|', L'/', L'|', L' ', 0x2014);
	
	/* CLI drawing */
	cli_frame->clear();
	fill_cli();
	fr::anim::slide_down(seconds_since_startup*0.8, *cli_frame, 5, sf::Color::Green, false);
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
		gv->draw(initex);
		cli_frame->draw(initex);
		initex->display();
		/* Shader passes */
		sf::RenderStates state;
		state.blendMode = sf::BlendAdd;
		blurtex->draw(sf::Sprite(initex->getTexture()), blur);
		blurtex->draw(sf::Sprite(initex->getTexture()), state);
		blurtex->display();
		 
		/* Draw the final RenderTexture to the window */
		win->clear();
		sf::Sprite intersprite(blurtex->getTexture());
		win->draw(intersprite);
		delete initex;
		delete blurtex;
	}
	else
	{
		/* Use direct rendering to the window */
		gv->draw(win);
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
	std::cout << margin << std::endl;
	SettingContainer s;
	int gv_w = size.x-s.get_sidebar_width()-margin*2; /* gameview width */
	if (gv_w < 0)
		gv_w = 0;
	 
	try
	{
		gv->set_origin(sf::Vector2i(margin, margin));
		gv->set_end(sf::Vector2i(gv_w, size.y-margin));
		cli_frame->set_origin(sf::Vector2i(gv_w, margin));
		cli_frame->set_end(sf::Vector2i(size.x-margin, size.y-margin));
	}
	catch(int e)
	{
		if (e == ERR)
		{
			/* Coming soon: A logger */
			std::cout << "Origin smaller than end" << std::endl;
		}
		
	}
}

fr::ObjRep in::GfxManager::drw_to_objrep(fa::Drawable drw)
{
	fr::ObjRep ret(drw.main.ch);
	if (drw.anims.any())
	{
		/* At least one animation is active */
		
		/* TODO: Try to use anim_is_active() here as well; It isn't in
		 * use right now because I'm currently too stupid to time it
		 * right with the cur_anim calculation down below;
		 */
		using namespace std::chrono;
		high_resolution_clock::time_point now = high_resolution_clock::now();
		if (duration_cast<seconds>(now.time_since_epoch()).count()%2 == 0)
		{
			/* An animation should play */
			
			/* A number between 0 and the number of active
			 * animations that describes the animation that should
			 * play now in the order of the bitset
			 * cur_anim=2;
			 *        |
			 * 0100100100
			 * 0123456789
			 * 
			 * => The third animation that is active but the actual
			 * animation to be played is nr. 7
			 */
			int cur_anim = (duration_cast<seconds>(now.time_since_epoch()).count()
				/2)%drw.anims.count();
			
			/* Find the cur_anim'th active animation in the bitset */
			int real_bits = 0;
			int anim_id = 0;
			for (int i = 0; i<drw.anims.size(); i++)
			{
				if (real_bits == cur_anim)
					anim_id = i;
				if (drw.anims[i] == 1)
					real_bits++; 
			}
			an::g::Anim animrep = an::g::anim_types.at(anim_id);
			if (animrep.ch != 0x0)
				ret.ch = animrep.ch;
			ret.fill = sf::Color(animrep.col.x, animrep.col.y, animrep.col.z, 255);
			ret.bg = sf::Color(animrep.bg.x, animrep.bg.y, animrep.bg.z, 255);
			return ret;
		}
	}
	ret.fill = sf::Color(drw.main.col.x, drw.main.col.y, drw.main.col.z, 255);
	ret.bg = sf::Color(drw.main.bg.x, drw.main.bg.y, drw.main.bg.z, 255);
	return ret;
}

void in::GfxManager::fill_gv()
{
	sf::Vector2i gvsize = gv->get_grid_size();
	/* This seems to be the most important performance bottleneck;
	 * Chunks would make sense
	 */
	for (ecs::entity_id ent : ecs::AggView<fa::Position, fa::Drawable>(*agg))
	{
		fa::Position* pos = agg->get_cmp<fa::Position>(ent);
		int x = pos->get_x() - cam_center.x + gvsize.x/2, 
				y = pos->get_y() - cam_center.y + gvsize.y/2;
		if (x >= 0 && x < gvsize.x && y >= 0 && y < gvsize.y)
		{
			fa::Drawable* rep = agg->get_cmp<fa::Drawable>(ent);
			gv->set_char(drw_to_objrep(*rep), x, y);
		}
	}
	/* For now, the function just rerenders everything that should be
	 * displayed above everything else (everything that is alive and
	 * player characters)
	 */
	for (ecs::entity_id ent : ecs::AggView<fa::Playable, fa::Alive, 
			fa::Position, fa::Drawable>(*agg))
	{
		fa::Position* pos = agg->get_cmp<fa::Position>(ent);
		int x = pos->get_x() - cam_center.x + gvsize.x/2, 
				y = pos->get_y() - cam_center.y + gvsize.y/2;
		if (x >= 0 && x < gvsize.x && y >= 0 && y < gvsize.y)
		{
			fa::Drawable* rep = agg->get_cmp<fa::Drawable>(ent);
			gv->set_char(drw_to_objrep(*rep), x, y);
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
			sf::Vector2i c = cli_frame->print(this_entry.c, 4, i, fr::EMPTY, true, 
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
		fr::ObjRep rep = fr::EMPTY;
		switch (this_entry.l)
		{
			case cli::INPUT:
				msg += L"> "; 
				x = 2;
				rep.bold = true;
				rep.fill = sf::Color(CLI_USER);
				break;
			case cli::DEBUG:
				rep = fr::ObjRep(L' ', sf::Color(128, 128, 128));
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
	fr::ObjRep s(CLI_PS1);
	if (cli_dat->get_active())
	{
		s.fill = sf::Color(CLI_ACTIVE);
		s.bg = sf::Color(CLI_ACTIVE, CLI_ALPHA);
		/* Box */
		for (int y = size_y-bottom_margin; y<size_y-1; y++)
		{
			for(int x = 2; x<size_x-2; x++)
			{
				cli_frame->set_char(fr::ObjRep(L' ', sf::Color::Black, sf::Color(CLI_ACTIVE, CLI_ALPHA)), 
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
				size_y-bottom_margin, {L' ', sf::Color(CLI_USER), sf::Color(CLI_ACTIVE, CLI_ALPHA)},
				true, size_x-3);
		/* "Cursor" */
		if (anim_is_active(1.0, 0.5))
		{
			cli_frame->set_char({L'|', sf::Color::White, 
					sf::Color(CLI_ACTIVE, CLI_ALPHA)}, endpos.x, endpos.y);
		}
	}
	else
	{
		cli_frame->print(cli_dat->get_bfr(), 4, 
				size_y-bottom_margin, {L' ', sf::Color(CLI_USER), sf::Color(0,0,0, CLI_ALPHA)});
	}
}

bool in::GfxManager::anim_is_active(float seconds_on, float seconds_off)
{
	int ms_on = seconds_on*1000; int ms_off = seconds_off*1000;
	using namespace std::chrono;
	high_resolution_clock::time_point now = high_resolution_clock::now();
	int ms_since_epoch = duration_cast<milliseconds>(now.time_since_epoch()).count();
	if (ms_since_epoch%(ms_on+ms_off) < ms_on)
	{
		return true;
	}
	return false;
}
