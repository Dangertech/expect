#pragma once
#include <vector>
#include <string>

struct Vec2
{
	int x;
	int y;
};

struct Vec3
{
	Vec3(int my_x, int my_y, int my_z)
	{
		x = my_x;
		y = my_y;
		z = my_z;
	}
	int x;
	int y;
	int z;
};

std::vector<std::wstring> split(std::wstring str);
template <typename T>
T smaller(T first, T second)
{
	return first < second ? first : second;
}
template <typename T>
T larger(T first, T second)
{
	return first > second ? first : second;
}
