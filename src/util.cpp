#include "util.hpp"
 
std::vector<std::wstring> split(std::wstring str)
{
	std::vector<std::wstring> ret;
	std::wstring cur;
	for (int i = 0; i<str.size(); i++)
	{
		if(str[i] == ' ')
		{
			ret.push_back(cur);
			cur = L"";
		}
		else
			cur += str[i];
	}
	ret.push_back(cur);
	return ret;
}
