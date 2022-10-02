#pragma once
#include "engine/central/entity.h"
#include "engine/central/storage.h"
#include "engine/utility/algorithm.h"

namespace Kodanuki
{

template <typename ... T>
std::vector<Entity> search_entities(Mapping& mapping,
	std::type_identity<std::tuple<T...>>)
{
	std::vector<Entity> intersection;
	sorted_intersection(std::back_inserter(intersection),
		getStorage<T>(mapping)->identifiers()...);
	return intersection;
}

template <typename T>
std::vector<Entity> search_entities(Mapping& mapping)
{
	return search_entities(mapping, std::type_identity<typename T::tuple>());
}

template <typename ... T>
std::tuple<T&...> get_component_reference_tuple(Mapping& mapping, int& id,
	std::type_identity<std::tuple<T...>>)
{
	return std::tie(std::any_cast<T&>(getStorage<T>(mapping)->get(id))...);
}

template <typename iterate_types>
struct EntityIterator
{
	EntityIterator(Mapping& mapping, std::vector<Entity> entities, int position)
		: mapping(mapping), entities(entities), position(position) {}
		
	EntityIterator begin()
	{
		return EntityIterator(mapping, entities, 0);
	}

	EntityIterator end()
	{
		return EntityIterator(mapping, entities, entities.size());
	}

	void operator++()
	{
		position++;
	}
		
	bool operator!=(const EntityIterator& other)
	{
		return position != other.position;
	}

	auto operator*()
	{
		int id = entities[position].value();
		return get_component_reference_tuple(mapping, id,
			std::type_identity<typename iterate_types::tuple>());
	}

private:
	Mapping& mapping;
	std::vector<Entity> entities;
	int position;
};

template <typename T>
bool remove_with_return(Entity entity)
{
	ECS::remove<T>(entity);
	return 0;
}

template <typename ... T>
void remove_entity_tags(Entity entity, std::type_identity<std::tuple<T...>>)
{
	(void) entity; // Case where sizeof...(T) == 0;
	using expander = bool[];
	(void) expander {0, remove_with_return<T>(entity)...};
}

template <typename T>
void remove_entity_tags(std::vector<Entity> entities)
{
	for (Entity entity : entities) {
		remove_entity_tags(entity, std::type_identity<typename T::tuple>());
	}
}

template <typename T>
bool update_with_return(Entity entity)
{
	ECS::update<T>(entity);
	return 0;
}

template <typename ... T>
void update_entity_tags(Entity entity, std::type_identity<std::tuple<T...>>)
{
	(void) entity; // Case where sizeof...(T) == 0;
	using expander = bool[];
	(void) expander {0, update_with_return<T>(entity)...};
}

template <typename T>
void update_entity_tags(std::vector<Entity> entities)
{
	for (Entity entity : entities) {
		update_entity_tags(entity, std::type_identity<typename T::tuple>());
	}
}

}
