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
	/*
	std::wstring move(std::vector<std::wstring> args, ecs::Aggregate& agg);
	*/
	RET zoom(std::vector<std::wstring> args, 
			in::GfxManager& gfx, SettingContainer& set);
	RET echo(std::vector<std::wstring> args);
	RET quit(std::vector<std::wstring> args);
	RET cli(std::vector<std::wstring> args, bool& skiptxt, cli::CliData& cli);
}
