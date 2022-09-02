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
		drw->alts.push_back({L'B', Vec3(255,0,0)});
		drw->alts.push_back({L'M', Vec3(0,255,0)});
		drw->alts.push_back({L'W', Vec3(0,0,255)});
		drw->main.col.x = 115; drw->main.col.y = 60; drw->main.col.z = 16;
		agg.add_cmp<Pickable>(ret);
		return ret;
	}
}
