#include "world.hpp"

ecs::Aggregate* WorldContainer::at(Vec3 pos)
{
	Vec2 lookup = {pos.x, pos.y};
	unsigned long idx = conv_vec(lookup);
	return at(idx);
}
 
ecs::Aggregate* WorldContainer::at(Vec2 pos)
{
	unsigned long idx = conv_vec(pos);
	return at(idx);
}
 
ecs::Aggregate* WorldContainer::at(int x, int y, int z)
{
	Vec2 lookup = {x, y};
	unsigned long idx = conv_vec(lookup);
	return at(idx);
}
 
ecs::Aggregate* WorldContainer::at(int x, int y)
{
	Vec2 lookup = {x, y};
	unsigned long idx = conv_vec(lookup);
	return at(idx);
}

ecs::Aggregate* WorldContainer::at(unsigned long idx)
{
	if (chunks.find(idx) == chunks.end())
	{
		/* A new chunk has to be created */
		chunks[idx] = new ecs::Aggregate;
		return chunks.at(idx);
	}
	else
		return chunks.at(idx);
}

unsigned long WorldContainer::conv_vec(Vec2 metapos)
{
	return (unsigned long) metapos.x << 32 | (unsigned long) metapos.y;
}
