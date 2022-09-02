/* The fabric framework contains all components
 * in the game
 */
#pragma once
#include "ecs.hpp"
#include "util.hpp"
#include <iostream>
 
namespace fa
{
	
	struct Position
	{
		/* Pretty useless, I know;
		 * Has to be refined to actually make
		 * sense
		 */
		public:
			int get_x() { return x; }
			void set_x(int my_x) { x = my_x; }
			int get_y() { return y; }
			void set_y(int my_y) { y = my_y;}
			void set(int my_x, int my_y)
			{
				set_x(my_x);
				set_y(my_y);
			}
		private:
			int x = 0, y = 0;
	};
	struct Drawable
	{
		/* One of the targets is to be independent of any
		 * underlying rendering infrastructure. For this reason,
		 * fr::ObjRep (or anything falling under the frama domain)
		 * must not be used in the game implementation. I need a different system
		 * for that 
		 */
		struct ObjRep
		{
			wchar_t ch;
			Vec3 col = Vec3(255, 255, 255);
			Vec3 bg = Vec3(0, 0, 0);
		} main;
		std::vector<ObjRep> alts;
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
	 
	ecs::entity_id deal_player(int x, int y, ecs::Aggregate& agg);
	ecs::entity_id deal_wall(int x, int y, ecs::Aggregate& agg);
	ecs::entity_id deal_item(int x, int y, ecs::Aggregate& agg);
}
