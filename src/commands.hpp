#pragma once
#include "ecs.hpp"
#include "infra.hpp"
#include "settings.hpp"

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
		RET ret;
		if(args.size() == 0)
		{
			ret.push_back(cli::LogEntry(L"Please provide either 'in' or 'out'", 
				cli::MESSAGE));
			return ret;
		}
		if (args[0] == L"in")
			gfx.adjust_zoom(set.get_zoom_step());
		else if (args[0] == L"out")
			gfx.adjust_zoom(-set.get_zoom_step());
		else
			ret.push_back(cli::LogEntry(L"Argument not recognized: " + args[0],
					cli::MESSAGE));
		for (int i = 1; i<args.size(); i++)
		{
			ret.push_back(
				cli::LogEntry(L"Argument not recognized: " + args[i], cli::MESSAGE));
		}
		return ret;
	}
}
