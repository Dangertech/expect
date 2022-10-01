#include "util.hpp"

bool Vec3::operator==(Vec3 const &cmp)
{
	if (this->x == cmp.x && this->y == cmp.y && this->z == cmp.z)
		return true;
	else
		return false;
}
 
bool Vec3::operator!=(Vec3 const &cmp)
{
	if (this->x != cmp.x || this->y != cmp.y || this->z != cmp.z)
		return true;
	else
		return false;
}
 
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
	if (cur != L"")
		ret.push_back(cur);
	return ret;
}
