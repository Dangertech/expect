/* Manages the in-game command line interface to be found on the
 * right side of the screen
 */
#pragma once
#include "frama.hpp"

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
	class CliData
	{
		public:
			/* Feed a new line of player input into the CLI 
			 * that will be processed
			 */
			void input(std::wstring ipt_string);
			/* Add an autonomous logging entry into the CLI */
			void log(LogEntry entry);
			int num_entries();
			LogEntry get_entry(int num);
		private:
			std::vector<LogEntry> entries;
	};
	 
	/* Manages the whole rendering (drawing to a frame) aspect
	 * of the CLI;
	 * Should only be used by the GfxManager
	 */
	class CliGraphics
	{
		public:
			CliGraphics(fr::Frame& my_frame, CliData& my_data);
			/* Draws the CliData to the assigned frame */
			void draw();
		private:
			fr::Frame* frame;
			CliData* data;
	};
}
