#include "fabric.hpp"
namespace fa
{
	ecs::entity_id deal_player(int x, int y, int z, wrld::WorldContainer& wrld)
	{
		ecs::Aggregate* agg = wrld.at(x,y);
		ecs::entity_id ret = agg->new_entity();
		Position* pos = agg->add_cmp<Position>(ret);
		pos->set(x, y, z);
		agg->add_cmp<Playable>(ret);
		agg->add_cmp<Alive>(ret)->health = 100;
		agg->add_cmp<Blocking>(ret);
		return ret;
	}
	ecs::entity_id deal_wall(int x, int y, int z, wrld::WorldContainer& wrld)
	{
		ecs::Aggregate* agg = wrld.at(x,y);
		ecs::entity_id ret = agg->new_entity();
		Position*  pos = agg->add_cmp<Position>(ret);
		pos->set(x, y, z);
		agg->add_cmp<Blocking>(ret);
		fa::Wall* w = agg->add_cmp<Wall>(ret);
		w->type = Wall::REDBRICK;
		agg->add_cmp<Paintable>(ret);
		agg->add_cmp<Flammable>(ret);
		return ret;
	}
	ecs::entity_id deal_item(int x, int y, int z, wrld::WorldContainer& wrld)
	{
		ecs::Aggregate* agg = wrld.at(x,y);
		ecs::entity_id ret = agg->new_entity();
		Position* pos = agg->add_cmp<Position>(ret);
		pos->set(x, y, z);
		agg->add_cmp<Pickable>(ret);
		Eatable* e = agg->add_cmp<Eatable>(ret);
		e->type = Eatable::SLIME_MOLD;
		return ret;
	}
}
