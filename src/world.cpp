#include "world.hpp"

namespace wrld
{
	ecs::Aggregate* WorldContainer::at(Vec3 pos)
	{
		Vec2 lookup = {pos.x, pos.y};
		unsigned long idx = postoidx(lookup);
		return at(idx);
	}
	 
	ecs::Aggregate* WorldContainer::at(Vec2 pos)
	{
		unsigned long idx = postoidx(pos);
		return at(idx);
	}
	 
	ecs::Aggregate* WorldContainer::at(int x, int y, int z)
	{
		Vec2 lookup = {x, y};
		unsigned long idx = postoidx(lookup);
		return at(idx);
	}
	 
	ecs::Aggregate* WorldContainer::at(int x, int y)
	{
		Vec2 lookup = {x, y};
		unsigned long idx = postoidx(lookup);
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
	
	unsigned long WorldContainer::postoidx(Vec2 pos)
	{
		/* Always truncate the result to the lower number in case
		 * of an uneven one, because the chunk origin is in the top
		 * left
		 */
		return conv_vec({(int)pos.x/chunk_size, (int)pos.y/chunk_size});
	}
	
	unsigned long WorldContainer::conv_vec(Vec2 pos)
	{
		return (unsigned long) pos.x << 32 | (unsigned long) pos.y;
	}
	
	std::vector<unsigned long> WorldContainer::get_chunk_idxs()
	{
		std::vector<unsigned long> ret;
		for (auto i : chunks)
		{
			ret.push_back(i.first);
		}
		return ret;
	}
}
