/* The fabric framework contains all components
 * in the game
 */
#pragma once
#include "ecs.hpp"
#include "util.hpp"
#include "anim.hpp"
 

namespace fa
{
	struct Position
	{
			int x = 0, y = 0, z = 0;
			void set(int mx, int my, int mz)
			{
				x = mx; y = my; z = mz;
			}
	};
	struct Drawable
	{
		/* Implementation data can be looked up in anim.hpp */
		 
		/* The base "Animation" that should be active
		 * most of the time */
		an::g::Anim main;
		/* All active animations for this Drawable */
		std::bitset<an::g::MAX_ANIMS> anims;
	};
	/* Other entities shouldn't differ
	 * from the player in any way, they all
	 * have inventory and stats, this
	 * component is only for marking an entity as
	 * playable (e.g. the player accesses it)
	 */
	struct Playable
	{
	};
	struct Alive
	{
		int health;
	};
	/* Entity can be added to inventories */
	struct Pickable
	{
	};
	/* Can't be walked over (without going up a height level */
	struct Blocking
	{
	};
	 
	ecs::entity_id deal_player(int x, int y, int z, ecs::Aggregate& agg);
	ecs::entity_id deal_wall(int x, int y, int z, ecs::Aggregate& agg);
	ecs::entity_id deal_item(int x, int y, int z, ecs::Aggregate& agg);
}
