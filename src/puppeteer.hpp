#pragma once
#include "ecs.hpp"
#include "util.hpp"

/* Lets game entities interact in an uniform way;
 * i.e if a character should pick something up, the
 * pickup function can simply be run on this entity
 */
 
/* General Arguments */
#define GARGS ecs::Aggregate& agg, ecs::entity_id
namespace pptr
{
		ecs::entity_id plr(ecs::Aggregate& agg); /* Get a player entity */
		 
		/* TODO: These functions let entities perform actions that can
		 * also impact other entities and
		 * check beforehand, if they can perform them;
		 * Returns 0 on success and various error codes to inform
		 * the game on what went wrong on failure
		 */
		int pickup(GARGS, Vec2 dir);
		int take_step(GARGS, Vec2 dir);
};
