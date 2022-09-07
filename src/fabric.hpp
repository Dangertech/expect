/* The fabric framework contains all components
 * in the game
 */
#pragma once
#include "ecs.hpp"
#include "util.hpp"
#include <iostream>
#include <unordered_map>

/* TODO: Unify the animation system (except the drw_to_objrep function) */
 
/* Define no animations with an ID greater than ANIM_MAX */
#define ANIM_MAX 20
#define ANIM_BURNING 1
#define ANIM_FROZEN 2
#define ANIM_ASLEEP 3
 
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
		/* data structure for the base representation of an object
		 * and all possible animations
		 */
		struct ObjRep
		{
			wchar_t ch;
			Vec3 col = Vec3(255, 255, 255);
			Vec3 bg = Vec3(0, 0, 0);
		};
		/* data structure the predefines all possible
		 * animations a character can have
		 * - If set to 0x0, the character displayed is to be the same
		 *   as the main character
		 */
		const std::unordered_map<int, ObjRep> anim_types =
		{
			{ANIM_BURNING, {L'f', Vec3(235, 134, 25), Vec3(255,0,0)}},
			{ANIM_FROZEN, {0x0, Vec3(30, 78, 97), Vec3(255,255,255)}},
			{ANIM_ASLEEP, {L'Z', Vec3(128, 128, 128), Vec3(0,0,0)}}
		};
		/* The base ObjectRepresentation that should be active
		 * most of the time */
		ObjRep main;
		/* All active animations for this Drawable */
		std::bitset<ANIM_MAX> anims;
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
