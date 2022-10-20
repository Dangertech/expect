#include "world.hpp"

ecs::Aggregate* WorldContainer::at(Vec3 pos)
{
	Vec2 lookup = {pos.x, pos.y};
	unsigned long idx = conv_vec(lookup);
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
