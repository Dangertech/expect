#pragma once
#include <cstdint>
#include <bitset>
#include <vector>
#include <SFML/Graphics.hpp>

/* This module does not contain an
 * implementation .cpp file because
 * it uses mostly template functions that
 * have to be defined where they are declared
 */

/* This is my own implementation of a tutorial found here:
 * https://www.david-colson.com/2020/02/09/making-a-simple-ecs.html
 */


namespace ecs
{
	/* The entity_id is a composition of the
	 * entity_idx and the entity_vers
	 */
	using entity_idx  = unsigned int;
	using entity_vers = unsigned int;
	using entity_id   = unsigned long long;
	
	const int MAX_ENTITIES = 5000; /* How many entities can exist */
	const int MAX_CMPS     = 50; /* How many component types can exist */
	 
	/* Infrastructure to manage entity ids;
	 * The first 32 bits of the id of an Entity 
	 * is the index, the following 32 bits the version number.
	 * The version number is used to make sure that
	 * every entity still has unique data 
	 * after an entity is destroyed
	 */
	entity_id create_entity_id(entity_idx idx, entity_vers vers)
	{
		return ((entity_id)idx << 32 | (entity_id)vers);
	}
	entity_id get_entity_idx(entity_id id)
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
	 
	
	/* The interface for the user;
	 * A closed ecosystem containing entities
	 */
	class Aggregate
	{
		public:
			entity_id new_entity()
			{
				if (!free_entities.empty())
				{
					entity_idx new_idx = free_entities.back();
					free_entities.pop_back(); /* Remove entry */
					entity_id new_id = 
						create_entity_id(new_idx, get_entity_vers(entities[new_idx].id));
					entities[new_idx].id = new_id;
					return entities[new_idx].id;
				}
				entities.push_back({entities.size(), std::bitset<MAX_CMPS>()});
				return entities.back().id;
			};
			
			void destroy_entity(entity_id e_id)
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
			
			template<typename T>
			void add_cmp(entity_id e_id)
			{
				/* Ensures the user can only access entities
				 * that still exist
				 */
				if (entities[get_entity_idx(e_id)].id != e_id)
					return;
				 
				int cmp_id = get_cmp_id<T>();
				/* Allocate an appropriate component pool */
				if (cmp_pools.size() <= cmp_id)
				{
					cmp_pools.resize(cmp_id+1, nullptr);
				}
				if (cmp_pools[cmp_id] == nullptr)
				{
					cmp_pools[cmp_id] = new CmpPool(sizeof(T));
				}
				/* Initialize entry in the component pool */
				T* cmp = new (cmp_pools[cmp_id]->get(e_id)) T();
				entities[get_entity_idx(e_id)].cmps.set(cmp_id);
			}
			 
			template<typename T>
			void rm_cmp(entity_id e_id)
			{
				if (entities[get_entity_idx(e_id)].id != e_id)
					return;
				int cmp_id = get_cmp_id<T>();
				entities[get_entity_idx(e_id)].cmps.reset(cmp_id);
			}
			
			template <typename T>
			T* get_cmp(entity_id e_id)
			{
				if (entities[get_entity_idx(e_id)].id != e_id)
					return;
				 
				int cmp_id = get_cmp_id<T>();
				if (!entities[get_entity_idx(e_id)].cmps.test(cmp_id))
					return nullptr;
				/* TODO: I have no idea what this does 
				 * I think I'm typecasting whatever I find
				 * at this location in the pool to the requested type
				 */
				T* cmp = static_cast<T*>(cmp_pools[cmp_id]->get(e_id));
				return cmp;
			}
		private:
			int cmp_counter = 0;
			 
			template <typename T> 
			int get_cmp_id()
			{
				/* */
				static int s_cmp_id = cmp_counter++;
				return s_cmp_id;
			};
			
			 
			struct Entity
			{
				entity_id id;
				std::bitset<MAX_CMPS> cmps;
			};
			 
			/* Ah Shit, that's hardcore memory allocation,
			 * I have no idea what I'm doing
			 * https://www.david-colson.com/assets/images/ecsArticle/Figure4.png
			 * explains the concept
			 */
			class CmpPool
			{
				public:
					CmpPool(size_t my_elsize)
					{
						elsize = my_elsize;
						pdata = new char[elsize * MAX_ENTITIES];
					}
					~CmpPool()
					{
						delete[] pdata;
					}
					 
					inline void* get(size_t index)
					{
						return pdata + (index * elsize);
					}
					 
					char* pdata = nullptr;
					size_t elsize = 0;
			};
			 
			std::vector<Entity> entities;
			std::vector<CmpPool*> cmp_pools;
			std::vector<entity_idx> free_entities;
			
			/* These classes can access everything in
			 * every Aggregate
			 */
			template<typename... cmp_types>
			friend class AggView;
			friend class Iterator;
	};
	
	/* A helper for the user to quickly
	 * iterate through entities with specific components
	 * Goal is this:
	 * for (entity_id ent : AggView<cmp_one, cmp_two>(scene))
	 * {
	 * 	//This loop runs through every enity ent with cmp_one
	 * 	//and cmp_two attached 
	 * }
	 */
	
	template<typename... cmp_types>
	class AggView
	{
		AggView(Aggregate& my_agg)
		{
			agg = &my_agg;
			 
			/* Iterating through all entities in the Aggregate */
			if (sizeof...(cmp_types) == 0)
				all = true;
			else
			{
				/* Set the bitmask with all requested component types */
				int cmp_ids[] = {0, get_id<cmp_types>() ... };
				for (int i = 1; i < (sizeof...(cmp_types) + 1); i++)
					cmps.set(cmp_ids[i]);
			}
		}
		
		Aggregate *agg;
		bool all = false;
		std::bitset<MAX_CMPS> cmps;
		
		struct Iterator
		{
			Iterator(Aggregate* my_agg, entity_idx my_idx, 
					std::bitset<MAX_CMPS> my_cmps,
					bool my_all) 
			{
				agg = my_agg; idx = my_idx;
				cmps = my_cmps; all = my_all;
			}
			Aggregate *agg;
			entity_idx idx;
			std::bitset<MAX_CMPS> cmps;
			bool all = false;

			bool valid_idx()
			{
				return
					is_entity_valid(agg->entities[idx].id) &&
					(all || cmps == (cmps & agg->entities[idx].cmps));
			}
			
			entity_id operator*() const
			{
				// give back the entityID we're currently at
				return agg->entities[idx].id;
			}
			
			bool operator==(const Iterator& other) const
			{
				// Compare two iterators
				return idx == other.idx || idx == agg->entities.size();
			}
			
			bool operator!=(const Iterator& other) const
			{
				// Similar to above
				return idx != other.idx && idx != agg->entities.size();
			}
			
			Iterator& operator++()
			{
				// Move the iterator forward until the next matching entity is reached
				do
				{
					idx++;
				} while (idx < agg-> entities.size() && !valid_idx());
				return *this;
			}
		};
		
		const Iterator begin() const
		{
			// Give an iterator to the beginning of this view
			int first_idx = 0;
			while (first_idx < agg->entities.size() &&
					(cmps != (cmps & agg->entities[first_idx].cmps)
					|| !is_entity_valid(agg->entities[first_idx].id)))
				first_idx++;
			return Iterator(agg, first_idx, cmps, all);
		}
		
		const Iterator end() const
		{
			// Give an iterator to the end of this view 
			return Iterator(agg, entity_idx(agg->entities.size()), cmps, all);
		}
	};
};
