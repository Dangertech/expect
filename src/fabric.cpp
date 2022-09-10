#include "fabric.hpp"
namespace fa
{
	ecs::entity_id deal_player(int x, int y, ecs::Aggregate& agg)
	{
		ecs::entity_id ret = agg.new_entity();
		Position* pos = agg.add_cmp<Position>(ret);
		pos->set(x, y);
		Drawable* drw = agg.add_cmp<Drawable>(ret);
		/* TODO: Library containing the representations
		 * of all possible objects
		 */
		drw->main.ch = L'@';
		agg.add_cmp<Playable>(ret);
		agg.add_cmp<Alive>(ret)->health = 100;
		agg.add_cmp<Blocking>(ret);
		return ret;
	}
	ecs::entity_id deal_wall(int x, int y, ecs::Aggregate& agg)
	{
		ecs::entity_id ret = agg.new_entity();
		Position*  pos = agg.add_cmp<Position>(ret);
		pos->set(x, y);
		Drawable* drw = agg.add_cmp<Drawable>(ret);
		drw->main.ch = L'#';
		agg.add_cmp<Blocking>(ret);
		return ret;
	}
	ecs::entity_id deal_item(int x, int y, ecs::Aggregate& agg)
	{
		ecs::entity_id ret = agg.new_entity();
		Position* pos = agg.add_cmp<Position>(ret);
		pos->set(x, y);
		Drawable* drw = agg.add_cmp<Drawable>(ret);
		drw->main.ch = L'&'; /* A loaf of bread? */
		drw->main.col.x = 115; drw->main.col.y = 60; drw->main.col.z = 16;
		drw->anims.set(an::g::AId::FROZEN, 1);
		drw->anims.set(an::g::AId::BURNING, 1);
		agg.add_cmp<Pickable>(ret);
		return ret;
	}
}
