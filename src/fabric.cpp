#include "fabric.hpp"
namespace fa
{
	void Position::set(Vec3 pos, wrld::WorldContainer& wrld)
	{
		int w_chunk_x = pos.x / CHUNK_SIZE;
		int w_chunk_y = pos.y / CHUNK_SIZE;
		if (w_chunk_x != chunk_x || w_chunk_y != chunk_y || !initialized)
		{
			/* TODO: OOH YEAH; SHUFFLE BABEH!
			 * The attached entity has to change chunks
			 */
			ecs::Aggregate* old_agg = wrld.at(get().x, get().y);
			ecs::Aggregate* new_agg = wrld.at(pos.x, pos.y);
		}
		z = pos.z;
	}
	Vec3 Position::get()
	{
		return {chunk_x*CHUNK_SIZE+local_x, chunk_y*CHUNK_SIZE+local_y, z};
	}
	ecs::entity_id deal_player(int x, int y, int z, wrld::WorldContainer& wrld)
	{
		ecs::Aggregate* agg = wrld.at(x,y);
		ecs::entity_id ret = agg->new_entity();
		Position* pos = agg->add_cmp<Position>(ret);
		pos->set({x, y, z}, wrld);
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
		pos->set({x, y, z}, wrld);
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
		pos->set({x, y, z}, wrld);
		agg->add_cmp<Pickable>(ret);
		Eatable* e = agg->add_cmp<Eatable>(ret);
		e->type = Eatable::SLIME_MOLD;
		return ret;
	}
}
