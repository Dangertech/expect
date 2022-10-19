#include "input.hpp"
#include "commands.hpp"

std::vector<cli::LogEntry> ipt::process_input
(
	std::wstring input,
	ecs::Aggregate& agg,
	cli::CliData& cli,
	in::GfxManager& gfx,
	SettingContainer& set,
	bool& skiptxt
)
{
	pptr::Puppetmaster pup(agg, gfx);
	std::vector<cli::LogEntry> response;
	std::vector<std::wstring> line = split(input);
	if (line.size() > 0)
	{
		std::vector<std::wstring> args = line;
		args.erase(args.begin()); /* remove the leftover command */
		if (line[0] == L"zoom")
			response = cmd::zoom(args, gfx, set);
		else if (line[0] == L"cli")
			response = cmd::cli(args, skiptxt, cli);
		else if (line[0] == L"echo")
			response = cmd::echo(args);
		else if (line[0] == L"quit")
			response = cmd::quit(args);
		else if (line[0] == L"move")
			response = cmd::move(args, pup);
		else if (line[0] == L"pickup")
			response = cmd::pickup(args, pup);
		else if (line[0] == L"become")
			response = cmd::become(args, gfx);
		else (response.push_back(
				cli::LogEntry(L"Command not recognized: " + line[0],
				cli::MESSAGE))
			);
	}
	return response;
}
