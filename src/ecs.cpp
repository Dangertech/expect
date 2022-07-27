#include "const.h"
#include "ecs.hpp"

/* Well, at least it clears like 60 lines... */

namespace ecs
{
	entity_id Aggregate::new_entity()
	{
		if (!free_entities.empty())
		{
			entity_idx new_idx = free_entities.back();
			free_entities.pop_back(); /* Remove entry */
			entity_id new_id = 
				create_entity_id(new_idx, get_entity_vers(entities[new_idx].id));
			entities[new_idx].id = new_id;
			if (entities.size() > MAX_ENTITIES)
				throw ERR_OVERFLOW;
			return entities[new_idx].id;
		}
		else
		{
			entities.push_back({create_entity_id(entity_idx(entities.size()), 0), 
					std::bitset<MAX_CMPS>()});
			if (entities.size() > MAX_ENTITIES)
				throw ERR_OVERFLOW;
			return entities.back().id;
		}
	}

	void Aggregate::destroy_entity(entity_id e_id)
	{
		/* Just assign the entity in this slot an invalid
		 * id and free its mask;
		 * Bump up the version number to mark that its a new
		 * entity now
		 */
		entity_id new_id = create_entity_id(entity_idx(-1), 
				get_entity_vers(e_id) +1);
		entities[get_entity_idx(e_id)].id = new_id;
		entities[get_entity_idx(e_id)].cmps.reset();
		/* Declare the entity as free again ;
		 * It will be used first if a new entity wants to
		 * be assigned
		 */
		free_entities.push_back(get_entity_idx(e_id));
	}

	entity_id create_entity_id(entity_idx idx, entity_vers vers)
	{
		return ((entity_id)idx << 32) | ((entity_id) vers);
	}
	entity_idx get_entity_idx(entity_id id)
	{
		/* Shift by 32 bits so the version number is pushed out */
		return id >> 32;
	}
	entity_vers get_entity_vers(entity_id id)
	{
		/* Just cast the id into the smaller format so the
		 * variable loses its first 32 bits
		 */
		return (entity_vers) id;
	}
	bool is_entity_valid(entity_id id)
	{
		return (id >> 32) != entity_idx(-1);
	}
}
