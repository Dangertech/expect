/* The fabric framework contains all components
 * in the game
 */
#include "frama.hpp"
#include "ecs.hpp"
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
		private:
			int x = 0, y = 0;
	};
	struct Drawable
	{
		public:
			fr::ObjRep get_rep() { return rep; }
			void set_rep(fr::ObjRep my_rep) { rep = my_rep; }
		private:
			fr::ObjRep rep = fr::EMPTY;
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
	 
	class EntityDealer
	{
		public:
			EntityDealer(ecs::Aggregate& my_agg)
			{
				agg = &my_agg;
			}
			ecs::entity_id deal_player(int x, int y)
			{
				ecs::entity_id ret = agg->new_entity();
				Position* pos = agg->add_cmp<Position>(ret);
				if (pos == nullptr)
					std::cout << "AH SHIT" << std::endl;
				pos->set_x(x);
				pos->set_y(y);
				Drawable* drw = agg->add_cmp<Drawable>(ret);
				/* TODO: Library containing the representations
				 * of all possible objects
				 */
				drw->set_rep(fr::ObjRep(L"@"));
				agg->add_cmp<Playable>(ret);
				return ret;
			}
		private:
			ecs::Aggregate* agg; 
	};
}
