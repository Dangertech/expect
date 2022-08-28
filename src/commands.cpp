#include "commands.hpp"

namespace cmd
{
	/*
	std::wstring move(std::vector<std::wstring> args, ecs::Aggregate& agg);
	*/
	RET zoom(std::vector<std::wstring> args, 
			in::GfxManager& gfx, SettingContainer& set)
	{
		/* This vector contains all arguments that couldn't be handled */
		std::vector<std::wstring> unhandled_args = args;
		RET ret;
		if(args.size() == 0)
		{
			ret.push_back(cli::LogEntry(L"Please provide either 'in' or 'out'", 
				cli::MESSAGE));
			return ret;
		}
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
	RET cli(std::vector<std::wstring> args, bool& incli, bool& skiptxt, cli::CliData& cli)
	{
		if (!args.size())
		{
			incli = !incli;
			cli.set_active(incli);
			return RET(); /* returns empty std::vector<cli::LogEntry> */
		}
		if (args[0] == L"enter") 
		{
			incli = true;
			skiptxt = true;
			cli.set_active(true);
		}
		else if (args[0] == L"exit")
		{
			incli = false;
			cli.set_active(false);
		}
		else
			return {{L"Argument not recognized: " + args[0], cli::MESSAGE}};
		return RET();
	}
}
