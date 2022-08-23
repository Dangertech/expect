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
			/* Add an autonomous logging entry into the CLI */
			void log(LogEntry entry);
			/* Get how many entries there are */
			int num_entries();
			/* Add a character to the input buffer 
			 * Returns true if the cli mode should be
			 * exited because enter (code 13) was pressed
			 */
			bool add_to_bfr(wchar_t ipt);
			/* Pushes whatever is in the buffer to processing and
			 * subsequently clears the buffer 
			 */
			void push_bfr();
			std::wstring get_bfr() { return bfr; }
			LogEntry get_entry(int num);
			
			bool get_active() { return active; }
			void set_active(bool a) { active = a; }
		private:
			/* Feed a new line of player input into the CLI 
			 * that will be logged and processed
			 */
			void input(std::wstring ipt_string);
			std::vector<LogEntry> entries;
			std::wstring bfr; /* Buffer containing currently typed input */
			bool active = false; /* Should be set to true if CLI mode is activated */
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
