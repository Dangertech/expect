#include "commands.hpp"
#include "puppeteer.hpp"
 
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
		ret.push_back(cli::LogEntry(errmsg));
		return true;
	}
	return false;
}
 
namespace cmd
{
	RET move(std::vector<std::wstring> args, pptr::Puppetmaster& pup)
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
			return {cli::LogEntry(helpmsg)};
		}
		 
		int response = pup.take_step(pup.plr(), dir);
		switch (response)
		{
			case 0:
				return {{}};
			case 1:
				return {{L"You are not in a dimensional plane of existence!"}};
			case 80:
				return {{L"There's something in the way!"}};
			default:
				return {{L"Undefined Error", cli::DEBUG}};
		}
	}
	 
	RET pickup(std::vector<std::wstring> args, pptr::Puppetmaster& pup)
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
				return {cli::LogEntry(dirhelp())};
			}
		}
		int response = pup.pickup(pup.plr(), dir);
		switch (response)
		{
			case 0:
				return {{L"You picked up an item!"}};
			case 1:
				return {{std::wstring(L"You are not in a dimensional plane of existence ")
					+ L"and thus can not pick anything up."}};
			case 80:
				return {{L"There is nothing to pick up here"}};
			default:
				return {{L"Undefined Error.", cli::DEBUG}};
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
				cli::LogEntry(L"Argument not recognized: " + args[i]));
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
			ret.push_back(cli::LogEntry(string)); 
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
			return {{L"Argument not recognized: " + args[0]}};
		return RET();
	}
	RET become(std::vector<std::wstring> args, in::GfxManager& gfx)
	{
		if (!args.size())
			return {{L"Please provide a setting you want to change!"},
				{L"Run 'help cmd become' for a list of settings!"}};
		if (args[0] == L"shaders")
		{
			gfx.set_shaders(!gfx.get_shaders());
			if (gfx.get_shaders())
				return {{L"Switched shaders on!"}};
			else
				return {{L"Switched shaders off!"}};
		}
		else if (args[0] == L"layers")
		{
			if (args.size() < 2)
			{
				return {{L"Specify how many gameview levels should be shown additionally",
					cli::MESSAGE}};
			}
			std::wstringstream ss;
			ss << args[1];
			int layers;
			if (!(ss >> layers))
				return {{L"Specify a number between 0 and 7."}}; 
			try
			{
				gfx.set_extra_layers(layers);
			}
			catch (int e)
			{
				return {{L"Specify a number between 0 and 7."}};
			}
			return RET();
		}
		else
		{
			return {{std::wstring(L"Argument not understood; Run 'help cmd become' ")
				+L"for help on this command!"}};
		}
	}
}
