/* Contains the memory for all entities in the world
 * by organizing a group of ECS-systems as chunks;
 */
#pragma once
#include <unordered_map>
#include "ecs.hpp"
#include "util.hpp"
#include "const.h"

namespace wrld
{
	class WorldContainer
	{
		public:
			ecs::Aggregate* at(Vec2 pos); 
			ecs::Aggregate* at(int x, int y); 
			ecs::Aggregate* at(unsigned long idx);
			unsigned long postoidx(Vec2 pos);
			int count_chunks(){return chunks.size();};
			std::vector<unsigned long> get_chunk_idxs();
		private:
			/* The key is a coordinate system corresponding to
			 * the position of the chunks; The first 32 bits contain
			 * the x, the second 32 bits the y position (encodable in
			 * conv_vec); The position on the top left of a chunk is given;
			 * This uniquely identifies every chunk;
			 * 
			 * However, the coordinate system is not the real, entity
			 * coordinate system (e.g. with a chunk size of 128 it goes
			 * in steps of 128 but instead it counts the chunks themselves
			 * as its grid to allow for insane world sizes.
			 */
			std::unordered_map<unsigned long, ecs::Aggregate*> chunks;
			/* Chunk size on the x and y axis; A chunk has an inifinite
			 * z axis;
			 */
			const int chunk_size = CHUNK_SIZE;
			/* Convert a vector2 into an unsigned long, with the 
			 * first 32 bits containing the x position and the last
			 * 32 bits containing the y position
			 */
			unsigned long conv_vec(Vec2 pos);
	};
	
	/* TODO: WorldView in some rare cases? */
	
	
	/* Contains everything needed to uniquely identify a single
	 * entity
	 */
	struct EntDescriptor
	{
		unsigned long chunk_idx;
		ecs::entity_id id;
	};
}
