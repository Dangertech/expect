#pragma once
#include "ecs.hpp"
#include "infra.hpp"
#include "settings.hpp"
#include <sstream>

#define RET std::vector<cli::LogEntry>
 
/* Contains all command "programs";
 * They can actually be thought as their own
 * programs that are given limited access to some APIs
 */
namespace cmd
{
	std::wstring move(std::vector<std::wstring> args, ecs::Aggregate& agg);
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
}
