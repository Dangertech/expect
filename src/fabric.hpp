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
		int health = 100;
	};
	/* Entity can be added to inventories */
	struct Pickable
	{
	};
	/* Can't be walked over (without going up a height level */
	struct Blocking
	{
	};
	struct Wall
	{
		enum Type {CONCRETE, REDBRICK};
		Type type = CONCRETE;;
	};
	struct Paintable
	{
		float visibility = 0.f; /* Evaluated between 0 and 100 */
		std::wstring msg;
		Vec3 color;
	};
	struct Eatable
	{
		int nutrition = 10;
		enum Type {RATION, SLIME_MOLD};
		Type type = RATION;
	};
	struct Flammable
	{
		/* A lot of nice stuff could go in here */
		bool burning = false;
	};
	 
	ecs::entity_id deal_player(int x, int y, int z, ecs::Aggregate& agg);
	ecs::entity_id deal_wall(int x, int y, int z, ecs::Aggregate& agg);
	ecs::entity_id deal_item(int x, int y, int z, ecs::Aggregate& agg);
}
