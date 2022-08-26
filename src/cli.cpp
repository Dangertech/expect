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
 
