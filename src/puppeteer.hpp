#pragma once
#include "ecs.hpp"
#include "infra.hpp"
#include "util.hpp"

/* Lets game entities interact in an uniform way;
 * i.e if a character should pick something up, the
 * pickup function can simply be run on this entity
 */
 
namespace pptr
{
	class Puppetmaster
	{
		public:
			Puppetmaster(ecs::Aggregate& magg, in::GfxManager& mgfx)
				: agg(magg)
				, gfx(mgfx)
				{}
			ecs::entity_id plr(); /* Get a player entity */
			 
			/* TODO: These functions let entities perform actions that can
			 * also impact other entities and
			 * check beforehand, if they can perform them;
			 * Returns 0 on success and various error codes to inform
			 * the game on what went wrong on failure
			 */
			/* 1: Entity does not have position component
			 * 80: There is nothing to be picked up at that position
			 * ERR_OVERFLOW: Invalid Vector was passed; 
			 *   must be between -1 and 1 on x and y
			 */
			int pickup(ecs::entity_id, Vec2 dir);
			/* 1: Entity does not have position component
			 * 80: Position blocked
			 * ERR_OVERFLOW: Invalid Vector was passed; 
			 *   must be between -1 and 1 on x and y
			 */
			int take_step(ecs::entity_id, Vec2 dir);
		private:
			ecs::Aggregate& agg;
			in::GfxManager& gfx;
	};
}
