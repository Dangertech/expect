#include "puppeteer.hpp"
#include "fabric.hpp"
#include "const.h"

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
		return ERR;
	}
	int take_step(GARGS, Vec2 dir)
	{
		return ERR;
	}
}
