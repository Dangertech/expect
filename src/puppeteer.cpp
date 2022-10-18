#include "puppeteer.hpp"
#include "fabric.hpp"
#include "const.h"
 
/* NOTE: Having to do THIS for thousands of entities
* would be inefficient as fuck.
* Solution: Chonky Boi Chunks???
*/
std::vector<ecs::entity_id> get_at_pos(int x, int y, int z,
		ecs::Aggregate& agg)
{
	std::vector<ecs::entity_id> entts = agg.get_entts();
	std::vector<ecs::entity_id> ret;
	for (int i = 0; i<entts.size(); i++)
	{
		fa::Position* pos = agg.get_cmp<fa::Position>(entts[i]);
		if (pos == nullptr)
			continue;
		if (pos->x == x && pos->y == y && pos->z == z)
			ret.push_back(entts[i]); 
	}
	return ret;
}

namespace pptr
{
	ecs::entity_id plr(ecs::Aggregate& agg)
	{
		for (ecs::entity_id ent : ecs::AggView<fa::Playable>(agg))
		{
			return ent;
		}
		return 0;
	}
	
	int pickup(GARGS, Vec2 dir)
	{
		fa::Position* pos = agg.get_cmp<fa::Position>(ent);
		/* TODO: Check if entity has inventory system */
		if (pos == nullptr)
			return 1;
		for (ecs::entity_id glent :
				get_at_pos(pos->x+dir.x, pos->y+dir.y, pos->z, agg))
		{
			if (agg.get_cmp<fa::Pickable>(glent))
			{
				/* TODO: Inventory and pickup code */
				agg.destroy_entity(glent);
				return 0;
			}
		}
		return 80;
	}
	 
	int take_step(GARGS, Vec2 dir)
	{
		fa::Position* pos = agg.get_cmp<fa::Position>(ent);
		if (pos == nullptr)
			return 1;
		bool blocking = false;
		for (ecs::entity_id glent :
				get_at_pos(pos->x+dir.x, pos->y+dir.y, pos->z, agg))
		{
			if (agg.get_cmp<fa::Blocking>(glent) != nullptr)
				blocking = true;
		}
		if (!blocking)
		{
			pos->x += dir.x;
			pos->y += dir.y;
			return 0;
		}
		return 80;
	}
}
