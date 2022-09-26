#include "commands.hpp"
 

/* TODO: THE SHITTIEST POSITION EVER FOR THIS FUNCTION */
/* NOTE: Having to do THIS for thousands of entities
 * would be inefficient as fuck.
 * Solution: Chonky Boi Chunks???
 */
std::vector<ecs::entity_id> get_at_pos(int x, int y, int z,
		std::vector<ecs::entity_id> entts, ecs::Aggregate& agg)
{
	std::vector<ecs::entity_id> ret;
	for (int i = 0; i<entts.size(); i++)
	{
		fa::Position* pos = agg.get_cmp<fa::Position>(entts[i]);
		if (pos == nullptr)
			continue;
		if (pos->x == x && pos->y == y && pos->z == z)
			ret.push_back(entts[i]); 
	}
	return ret;
}

/* Resolves a direction name (like up, southeast or 1) to a relative position */
Vec2 nametopos(std::wstring name)
{
	Vec2 dir = {0, 0};
	if (name == L"up" || name == L"north" || name == L"8")
		dir.y = -1;
	else if (name == L"northeast" || name == L"9")
	{
		dir.x = 1;
		dir.y = -1;
	}
	else if (name == L"right" || name == L"east" || name == L"6")
		dir.x = 1;
	else if (name == L"southeast" || name == L"3")
	{
		dir.x = 1;
		dir.y = 1;
	}
	else if (name == L"down" || name == L"south" || name == L"2")
		dir.y = 1;
	else if (name == L"southwest" || name == L"1")
	{
		dir.x = -1;
		dir.y = 1;
	}
	else if (name == L"left" || name == L"west" || name == L"4")
		dir.x = -1;
	else if (name == L"northwest" || name == L"7")
	{
		dir.x = -1;
		dir.y = -1;
	}
	else
	{
		throw ERR;
	}
	return dir;
}

std::wstring dirhelp()
{
	return std::wstring(L"Please provide either a direction ")
			+ L"(e.g. 'left', 'northwest') or a number corresponding "
			+ L"to the directions on the numpad.";
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
		std::wstring helpmsg = dirhelp();
		RET ret;
		if (checkargs(args, ret, helpmsg))
			return ret;
		 
		Vec2 dir = {0, 0};
		try
		{
			dir = nametopos(args[0]);
		}
		catch (int e)
		{
			ret.push_back(cli::LogEntry(helpmsg, cli::MESSAGE));
		}
		 
		for (ecs::entity_id ent : ecs::AggView<fa::Position, fa::Playable>(agg))
		{
			fa::Position* pos = agg.get_cmp<fa::Position>(ent);
			bool blocking = false;
			for (ecs::entity_id glent : 
					get_at_pos(pos->x+dir.x, pos->y+dir.y, pos->z, entts, agg))
			{
				if (agg.get_cmp<fa::Blocking>(glent) != nullptr)
					blocking = true;
			}
			if (!blocking)
			{
				pos->x += dir.x;
				pos->y += dir.y;
			}
		}
		return ret;
	}
	 
	RET pickup(std::vector<std::wstring> args, std::vector<ecs::entity_id> entts, ecs::Aggregate& agg)
	{
		Vec2 dir = {0, 0};
		/* Get direction */
		if (!args.size())
			dir = {0, 0}; /* Try to pick up below yourself */
		else
		{
			try
			{
				dir = nametopos(args[0]);
			}
			catch (int e)
			{
				return {cli::LogEntry(dirhelp(), cli::MESSAGE)};
			}
		}
		ecs::entity_id plr;
		bool exists = false;
		for (ecs::entity_id ent : ecs::AggView<fa::Position, fa::Playable>(agg))
		{
			plr = ent;
			exists = true;
		}
		if (!exists)
			return {cli::LogEntry(
					L"You are dead and can not pick anything up.", cli::MESSAGE)};
		fa::Position* pos = agg.get_cmp<fa::Position>(plr);
		std::vector<ecs::entity_id> pot_targets = /* Potential Targets */
			get_at_pos(pos->x+dir.x, pos->y+dir.y, pos->z, entts, agg);
		std::vector<ecs::entity_id> targets;
		for (ecs::entity_id ent : pot_targets)
		{
			if(agg.get_cmp<fa::Pickable>(ent)) 
				targets.push_back(ent);
		}
		 
		if (!targets.size())
			return {cli::LogEntry(
					L"There is nothing to pick up.", cli::MESSAGE)};
		else
		{
			/* TODO: Actual pickup code */
			agg.destroy_entity(targets[0]);
			return {cli::LogEntry(
					L"You picked up an item!", cli::MESSAGE)};
		}
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
