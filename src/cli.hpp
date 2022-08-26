/* Manages the in-game command line interface to be found on the
 * right side of the screen
 */
#pragma once
#include "frama.hpp"
#include "ecs.hpp"

namespace cli
{
	enum LogType {INPUT, DEBUG, MESSAGE};
	struct LogEntry
	{
		std::wstring c/*ontent*/;
		LogType l;
		LogEntry(std::wstring my_c, LogType my_l)
		{
			c = my_c;
			l = my_l;
		}
	};

	/* Manages I/O capabilities of the CLI */
	/*
	 * Although it might seem like the CliData class
	 * also manages command processing, this is not true,
	 * it only contains data for display that has no real
	 * effect; Command functions can instead be found in
	 * commands.hpp
	 */
	class CliData
	{
		public:
			/* Add an autonomous logging entry into the CLI; */
			void log(LogEntry entry);
			/* Feed a new line of player input into the CLI;
			 * This just logs this line and has no real effect
			 */
			void input(std::wstring ipt_string);
			/* Get how many entries there are */
			int num_entries();
			/* Add a character to the input buffer 
			 * Returns true if the cli mode should be
			 * exited because enter (code 13) was pressed
			 */
			void add_to_bfr(wchar_t ipt);
			/* Pushes whatever is in the buffer to processing and
			 * subsequently clears the buffer 
			 */
			std::wstring push_bfr();
			std::wstring get_bfr() { return bfr; }
			LogEntry get_entry(int num);
			
			bool get_active() { return active; }
			void set_active(bool a) { active = a; }
		private:
			std::vector<LogEntry> entries;
			std::wstring bfr; /* Buffer containing currently typed input */
			bool active = false; /* Should be set to true if CLI mode is activated */
	};
	 
	/* The ability to display the CLI is in the GfxManager class */
}
