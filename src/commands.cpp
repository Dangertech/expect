#include "commands.hpp"
 

/* TODO: THE SHITTIEST POSITION EVER FOR THIS FUNCTION */
/* NOTE: Having to do THIS for thousands of entities
 * would be inefficient as fuck.
 * Solution: Chonky Boi Chunks???
 */
std::vector<ecs::entity_id> get_at_pos(int x, int y, 
		std::vector<ecs::entity_id> entts, ecs::Aggregate& agg)
{
	std::vector<ecs::entity_id> ret;
	for (int i = 0; i<entts.size(); i++)
	{
		fa::Position* pos = agg.get_cmp<fa::Position>(entts[i]);
		if (pos == nullptr)
			continue;
		if (pos->get_x() == x && pos->get_y() == y)
			ret.push_back(entts[i]); 
	}
	return ret;
}

/* Helper function to easily print an error message if a command has no arguments */
bool checkargs(std::vector<std::wstring> args, std::vector<cli::LogEntry>& ret, std::wstring errmsg)
{
	if (args.size() == 0)
	{
		ret.push_back(cli::LogEntry(errmsg, cli::MESSAGE));
		return true;
	}
	return false;
}
 
namespace cmd
{
	RET move(std::vector<std::wstring> args, std::vector<ecs::entity_id> entts, ecs::Aggregate& agg)
	{
		std::wstring helpmsg = std::wstring(L"Please provide either a direction ")
			+ L"(e.g. 'left', 'northwest') or a number corresponding "
			+ L"to the directions on the numpad.";
		RET ret;
		if (checkargs(args, ret, helpmsg))
			return ret;
		 
		Vec2 dir = {0, 0};
		if (args[0] == L"up" || args[0] == L"north" || args[0] == L"8")
			dir.y = -1;
		else if (args[0] == L"northeast" || args[0] == L"9")
		{
			dir.x = 1;
			dir.y = -1;
		}
		else if (args[0] == L"right" || args[0] == L"east" || args[0] == L"6")
			dir.x = 1;
		else if (args[0] == L"southeast" || args[0] == L"3")
		{
			dir.x = 1;
			dir.y = 1;
		}
		else if (args[0] == L"down" || args[0] == L"south" || args[0] == L"2")
			dir.y = 1;
		else if (args[0] == L"southwest" || args[0] == L"1")
		{
			dir.x = -1;
			dir.y = 1;
		}
		else if (args[0] == L"left" || args[0] == L"west" || args[0] == L"4")
			dir.x = -1;
		else if (args[0] == L"northwest" || args[0] == L"7")
		{
			dir.x = -1;
			dir.y = -1;
		}
		else
			ret.push_back(cli::LogEntry(helpmsg, cli::MESSAGE));
		 
		for (ecs::entity_id ent : ecs::AggView<fa::Position, fa::Drawable,
				fa::Playable>(agg))
		{
			fa::Position* pos = agg.get_cmp<fa::Position>(ent);
			bool blocking = false;
			for (ecs::entity_id glent : 
					get_at_pos(pos->get_x()+dir.x, pos->get_y()+dir.y, entts, agg))
			{
				if (agg.get_cmp<fa::Blocking>(glent) != nullptr)
					blocking = true;
			}
			if (!blocking)
			{
				pos->set_x(pos->get_x()+dir.x);
				pos->set_y(pos->get_y()+dir.y);
			}
		}
		return ret;
	}
	RET zoom(std::vector<std::wstring> args, 
			in::GfxManager& gfx, SettingContainer& set)
	{
		/* This vector contains all arguments that couldn't be handled */
		std::vector<std::wstring> unhandled_args = args;
		RET ret;
		if (checkargs(args, ret, L"Please provide either 'in' or 'out'"))
			return ret;
		int repeats = 1;
		if (args.size() > 1)
		{
			std::wstringstream ss;
			ss << args[1];
			if (ss >> repeats) /* Operation successful*/
				unhandled_args.erase(unhandled_args.begin()+1);
			else
				repeats = 1;
		}
		if (args[0] == L"in")
		{
			gfx.adjust_zoom(set.get_zoom_step()*repeats);
			unhandled_args.erase(unhandled_args.begin());
		}
		else if (args[0] == L"out")
		{
			gfx.adjust_zoom(-set.get_zoom_step()*repeats);
			unhandled_args.erase(unhandled_args.begin());
		}
		 
		for (int i = 1; i<unhandled_args.size(); i++)
		{
			ret.push_back(
				cli::LogEntry(L"Argument not recognized: " + args[i], cli::MESSAGE));
		}
		return ret;
	}
	RET echo(std::vector<std::wstring> args)
	{
		RET ret;
		std::wstring string;
		for (int i = 0; i<args.size(); i++)
		{
			string += args[i] + L" ";
		}
			ret.push_back(cli::LogEntry(string, cli::MESSAGE)); 
		return ret;
	}
	RET quit(std::vector<std::wstring> args)
	{
		exit(0); 
		return {{L"Quitting was not successful!", cli::DEBUG}};
	}
	RET cli(std::vector<std::wstring> args, bool& skiptxt, cli::CliData& cli)
	{
		if (!args.size())
		{
			cli.set_active(!cli.get_active());
			return RET(); /* returns empty std::vector<cli::LogEntry> */
		}
		if (args[0] == L"enter") 
		{
			skiptxt = true;
			cli.set_active(true);
		}
		else if (args[0] == L"exit")
		{
			cli.set_active(false);
		}
		else
			return {{L"Argument not recognized: " + args[0], cli::MESSAGE}};
		return RET();
	}
}
