#include "cli.hpp" 

void cli::CliData::log(cli::LogEntry entry)
{
	entries.push_back(entry);
	return;
}

void cli::CliData::input(std::wstring ipt_string)
{
	/* Show the message as a log entry */
	cli::LogEntry entry(ipt_string, INPUT);
	entries.push_back(entry);
	/* TODO: Actual input processing HERE */
}

bool cli::CliData::add_to_bfr(wchar_t ipt)
{
	if (ipt == 0)
		return false;
	if (ipt == 8) /* Backspace */
	{
		if (bfr.size() > 0)
			bfr.pop_back();
		return false;
	}
	if (ipt == 13) /* Enter */
	{
		push_bfr();
		return true;
	}
	bfr += ipt;
	return false;
}

void cli::CliData::push_bfr()
{
	input(bfr);
	bfr.clear();
}

int cli::CliData::num_entries()
{
	return entries.size();
}

cli::LogEntry cli::CliData::get_entry(int num)
{
	if (num < num_entries() && num >= 0)
		return entries[num];
	else
		throw ERR_OVERFLOW;
}

cli::CliGraphics::CliGraphics(fr::Frame& my_frame, CliData& my_data)
{
	frame = &my_frame;
	data = &my_data;
}

void cli::CliGraphics::draw()
{
	frame->clear();
	int num_entries = data->num_entries();
	int size_y = frame->get_grid_size().y;
	int bottom_margin = 3;
	for (int i = 0; i<size_y-bottom_margin; i++)
	{
		LogEntry this_entry(L" ", DEBUG);
		try
		{
			this_entry = data->get_entry(num_entries+i-size_y+bottom_margin);
		}
		catch (int e)
		{
		}
		std::wstring msg;
		int x = 4;
		fr::ObjRep rep = fr::EMPTY;
		switch (this_entry.l)
		{
			case INPUT:
				msg += L"> "; 
				x = 2;
				rep.bold = true;
				rep.fill = sf::Color::Green;
				break;
			case DEBUG:
				rep = fr::ObjRep(L' ', sf::Color(128, 128, 128));
				break;
			case MESSAGE:
				break;
		}
		msg += this_entry.c;
		frame->print(msg, x, i, rep);
	}
	/* Draw the "PS1" of the CLI input */
	fr::ObjRep s(L':');
	if (data->get_active())
		s.fill = sf::Color::Green;
	frame->set_char(s, 2, size_y-bottom_margin);
	frame->print(data->get_bfr(), 4, size_y-bottom_margin, s);
}
