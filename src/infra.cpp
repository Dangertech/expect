#include "infra.hpp"
#include "frama.hpp"
#include "const.h"
#include "settings.hpp"
#include <iostream>


in::GfxManager::GfxManager(ecs::Aggregate& my_agg, cli::CliData& my_cli_dat)
{
	/* I'm using pointers because I couldn't figure out
	 * how to initialize objects directly in a constructor
	 */
	agg = &my_agg;
	cli_dat = &my_cli_dat;
	win = new sf::RenderWindow(sf::VideoMode(1920, 1080), "expect");
	font = new sf::Font;
	bloom = new sf::Shader;
	if (!bloom->loadFromFile("bloom.frag", sf::Shader::Fragment))
	{
		std::cout << "Graphics Initialization Error: Bloom shader could "
			<< "not be initialized!" << std::endl;
	}
	font->loadFromFile("font.otb");
	gv = new fr::Frame(*win, *font, 32, sf::Vector2i(0, 0), sf::Vector2i(5, 5));
	cli_frame = new fr::Frame(*win, *font, 32, sf::Vector2i(0, 0), sf::Vector2i(5, 5));
	cli_graphics = new cli::CliGraphics(*cli_frame, *cli_dat);
	/* Setting this doesn't matter:
	 * When win.display() is called, sfml just waits for the time remaining
	 * to fulfill the fps requirement; However, by main() design, win.display()
	 * is skipped on almost every iteration of the main loop which makes the
	 * fps setting useless. Instead, the fps are defined in settings.hpp
	 * and executed in the main loop (or somewhere else in the future)
	 */
	//win->setFramerateLimit(30);
	gv->set_frame_bg(sf::Color::Black);
	cli_frame->set_frame_bg(sf::Color::Black);
	cli_frame->set_standard_scale(0.6f);
	update_sizes();
}
in::GfxManager::~GfxManager()
{
	delete win;
	delete font;
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
			queue_render = true;
		}
		if (e.type == sf::Event::GainedFocus)
			queue_render = true;
		/* Push back into a simulated queue to be batch
		 * processed externally
		 */
		ret.push_back(e);
	}
	
	return ret;
}

bool in::GfxManager::render_gv(bool force)
{
	if (!force && !queue_render)
		return false;
	gv->clear();
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
	queue_render = false;
	/* Draw CLI */
	cli_graphics->draw();
	return true;
}

bool in::GfxManager::display_frames()
{
	bool updated = false;
	if (gv->draw(bloom))
	{
		std::cout << "Updated game viewport!" << std::endl;
		updated = true;
	}
	if (cli_frame->draw(bloom))
	{
		std::cout << "Updated Sidebar Viewport!" << std::endl;
		updated = true;
	}
	win->display();
	return updated;
}

void in::GfxManager::update_sizes()
{
	sf::Vector2u size = win->getSize();
	SettingContainer s;
	int gv_w = size.x-s.get_sidebar_width(); /* gameview width */
	if (gv_w < 0)
		gv_w = 0;
	 
	try
	{
		gv->set_origin(sf::Vector2i(0,0));
		gv->set_end(sf::Vector2i(gv_w, size.y));
		cli_frame->set_origin(sf::Vector2i(gv_w, 0));
		cli_frame->set_end(sf::Vector2i(size.x, size.y));
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
	fr::ObjRep ret(drw.ch);
	ret.fill = sf::Color(drw.col.x, drw.col.y, drw.col.z, 255);
	return ret;
}
