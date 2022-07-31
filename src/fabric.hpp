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
		wchar_t ch;
		Vec3 col = Vec3(255, 255, 255);
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
	 
	class EntityDealer
	{
		public:
			EntityDealer(ecs::Aggregate* my_agg)
			{
				agg = my_agg;
			}
			ecs::entity_id deal_player(int x, int y)
			{
				ecs::entity_id ret = agg->new_entity();
				Position* pos = agg->add_cmp<Position>(ret);
				pos->set(x, y);
				Drawable* drw = agg->add_cmp<Drawable>(ret);
				/* TODO: Library containing the representations
				 * of all possible objects
				 */
				drw->ch = L'@';
				agg->add_cmp<Playable>(ret);
				agg->add_cmp<Alive>(ret)->health = 100;
				agg->add_cmp<Blocking>(ret);
				return ret;
			}
			ecs::entity_id deal_wall(int x, int y)
			{
				ecs::entity_id ret = agg->new_entity();
				Position*  pos = agg->add_cmp<Position>(ret);
				pos->set(x, y);
				Drawable* drw = agg->add_cmp<Drawable>(ret);
				drw->ch = L'#';
				agg->add_cmp<Blocking>(ret);
				return ret;
			}
			ecs::entity_id deal_item(int x, int y)
			{
				ecs::entity_id ret = agg->new_entity();
				Position* pos = agg->add_cmp<Position>(ret);
				pos->set(x, y);
				Drawable* drw = agg->add_cmp<Drawable>(ret);
				drw->ch = L'&'; /* A loaf of bread? */
				drw->col.y = 0; drw->col.z = 0;
				agg->add_cmp<Pickable>(ret);
				return ret;
			}
		private:
			ecs::Aggregate* agg; 
	};
}
