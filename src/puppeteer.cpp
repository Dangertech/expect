#include "puppeteer.hpp"
#include "fabric.hpp"
#include "const.h"
 
std::vector<ecs::entity_id> get_at_pos(Vec3 pos,
		ecs::Aggregate& agg)
{
	std::vector<ecs::entity_id> entts = agg.get_entts();
	std::vector<ecs::entity_id> ret;
	for (int i = 0; i<entts.size(); i++)
	{
		fa::Position* entpos = agg.get_cmp<fa::Position>(entts[i]);
		if (entpos == nullptr)
			continue;
		if (entpos->get() == pos)
			ret.push_back(entts[i]); 
	}
	return ret;
}

namespace pptr
{
	wrld::EntDescriptor Puppetmaster::plr()
	{
		std::vector<unsigned long> ch_idx = wrld.get_chunk_idxs();
		for (int i = 0; i<ch_idx.size(); i++)
		{
			ecs::Aggregate* tagg = wrld.at(ch_idx[i]);
			for (ecs::entity_id ent : ecs::AggView<fa::Playable>(*tagg))
			{
				return {ch_idx[i], ent};
			}
		}
		return {0,0};/* TODO: Better handling in case of no player */
	}
	
	int Puppetmaster::pickup(wrld::EntDescriptor ent, Vec2 dir)
	{
		ecs::Aggregate* agg = wrld.at(ent.chunk_idx);
		if (dir.x > 1 || dir.x < -1 || dir.y > 1 || dir.y < -1)
			return ERR_OVERFLOW;
		fa::Position* pos = agg->get_cmp<fa::Position>(ent.id);
		/* TODO: Check if entity has inventory system */
		if (pos == nullptr)
			return 1;
		for (ecs::entity_id glent : get_at_pos(pos->get(), *agg))
		{
			if (wrld.at(ent.chunk_idx)->get_cmp<fa::Pickable>(glent))
			{
				/* TODO: Inventory and pickup code */
				agg->destroy_entity(glent);
				gfx.gschanged();
				return 0;
			}
		}
		return 80;
	}
	 
	int Puppetmaster::take_step(wrld::EntDescriptor ent, Vec2 dir)
	{
		ecs::Aggregate* agg = wrld.at(ent.chunk_idx);
		if (dir.x > 1 || dir.x < -1 || dir.y > 1 || dir.y < -1)
			return ERR_OVERFLOW;
		fa::Position* pos = agg->get_cmp<fa::Position>(ent.id);
		if (pos == nullptr)
			return 1;
		bool blocking = false;
		for (ecs::entity_id glent : get_at_pos(pos->get(), *agg))
		{
			if (agg->get_cmp<fa::Blocking>(glent) != nullptr)
				blocking = true;
		}
		if (!blocking)
		{
			pos->set(pos->get().x + dir.x, pos->get().y + dir.y, pos->get().z);
			/* From here on, the given EntityDescriptor might be invalid */
			gfx.gschanged();
			return 0;
		}
		return 80;
	}
}
