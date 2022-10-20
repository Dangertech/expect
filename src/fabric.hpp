/* The fabric framework contains all components
 * in the game
 */
#pragma once
#include "ecs.hpp"
#include "util.hpp"
#include "anim.hpp"
#include "world.hpp"
 

namespace fa
{
	/* With the chunk system (world.hpp), Positions are
	 * an interesting matter;
	 */
	struct Position
	{
		public:
			/* Set world coordinate positions */
			void set(Vec3 pos, wrld::WorldContainer& wrld);
			//void set(int x, int y, int z);
			Vec3 get(); /* Get world coordinate positions */
		private:
			int local_x = 0, local_y = 0, z = 0;
			/* Must NEVER go out of sync with the actual chunk the
			 * attached entity is in
			 */
			int chunk_x = 0, chunk_y = 0;
			bool initialized = false;
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
	 

	ecs::entity_id deal_player(int x, int y, int z, wrld::WorldContainer&);
	ecs::entity_id deal_wall(int x, int y, int z, wrld::WorldContainer&);
	ecs::entity_id deal_item(int x, int y, int z, wrld::WorldContainer&);
}
