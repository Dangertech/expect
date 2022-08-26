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


void cli::CliData::add_to_bfr(wchar_t ipt)
{
	if (ipt == 0)
		return;
	if (ipt == 8) /* Backspace */
	{
		if (bfr.size() > 0)
			bfr.pop_back();
		return;
	}
	if (ipt == 13) /* Enter MUST be filtered out beforehand */
		throw ERR;
	bfr += ipt;
}

std::wstring cli::CliData::push_bfr()
{
	std::wstring ret;
	input(bfr);
	ret = bfr;
	bfr.clear();
	return ret;
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
	int size_x = frame->get_grid_size().x;
	int bottom_margin = 3;
	/* Dry test to determine how many lines are needed extra for
	 * multiline entries
	 */
	int extra = 0;
	for (int i = 0; i<size_y - bottom_margin; i++)
	{
			LogEntry this_entry(L"", DEBUG);
			try
			{
				this_entry = data->get_entry(num_entries+i-size_y+bottom_margin);
			}
			catch (int e)
			{
			}
			sf::Vector2i c = frame->print(this_entry.c, 4, i, fr::EMPTY, true, true);
			if (c.y != i)
				extra += c.y-i;
	}
	 
	/* Location of the current entry in the vector */
	int eloc = num_entries-size_y+bottom_margin+extra;
	for (int i = 0; i<size_y-bottom_margin; i++)
	{
		LogEntry this_entry(L"", DEBUG);
		try
		{
			this_entry = data->get_entry(eloc);
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
		sf::Vector2i c(x,i);
		if (msg.size())
		{
			/* Now print for real */
			c = frame->print(msg, x, i, rep, true, false);
		}
		i = c.y;
		eloc += 1;
	}
	 
	/* Draw the "PS1" of the CLI input */
	fr::ObjRep s(L':');
	if (data->get_active())
	{
		s.fill = sf::Color::Green;
		s.bg = sf::Color(0,255,0,80);
		for (int y = size_y-bottom_margin; y<size_y-1; y++)
		{
			for(int x = 2; x<size_x-2; x++)
			{
				frame->set_char(fr::ObjRep(L' ', sf::Color::Black, sf::Color(0,255,0,80)), 
						x, y);
			}
		}
	}
	frame->set_char(s, 2, size_y-bottom_margin);
	frame->print(data->get_bfr(), 4, size_y-bottom_margin, s);
}
