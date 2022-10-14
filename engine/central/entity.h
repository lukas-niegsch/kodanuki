#pragma once
#include "engine/central/storage.h"
#include <cstdint>
#include <optional>
#include <any>

namespace Kodanuki
{

/**
 * Each entity in the ECS defines a unique identifier.
 * 
 * Multiple components can be attached to an entity. These components are
 * collections of data. One or more components together form an archetype.
 * Systems can iterate over archetypes to change all entities at once.
 */
typedef std::optional<uint64_t> Entity;

/**
 * Functions for interating with the ECS directly.
 * 
 * Entities can be passed throught the program, but each system maintains
 * its own components. Each system must remove these components or provide
 * a cleanup strategy. Neglecting this will impact performance.
 */
class ECS_t
{
public:
	// Creates a new entity with a unique identifier.
	Entity create();

	// Updates the given component inside the entity.
	template <typename T>
	void update(Entity entity, T component = {})
	{
		storage<T>()->update(entity.value(), component);
	}

	// Removes the given component from the entity.
	template <typename T>
	void remove(Entity entity)
	{
		if constexpr (std::is_same<T, Entity>()) {
			clear(entity);
		} else {
			storage<T>()->remove(entity.value());
		}
	}

	// Returns true iff the entity has the given component.
	template <typename T>
	bool has(Entity entity)
	{
		return storage<T>()->contains(entity.value());
	}

	// Returns the reference to the component.
	template <typename T>
	T& get(Entity entity)
	{
		std::any& component = storage<T>()->get(entity.value());
		return std::any_cast<T&>(component);
	}

	// Copies the component from the source entity to the target entity.
	template <typename T>
	void copy(Entity source, Entity target)
	{
		storage<T>()->copy(source.value(), target.value());
	}

	// Moves the component from the source entity to the target entity.
	template <typename T>
	void move(Entity source, Entity target)
	{
		storage<T>()->move(source.value(), target.value());
	}

	// Swaps the component from the source entity with the target entity.
	template <typename T>
	void swap(Entity source, Entity target)
	{
		storage<T>()->swap(source.value(), target.value());
	}

	// Binds the component from the source entity to the target entity.
	template <typename T>
	void bind(Entity source, Entity target)
	{
		storage<T>()->bind(source.value(), target.value());
	}

	// Iterates over entities with the given archetype.
	template <typename Archetype>
	auto iterate()
	{
		return Archetype::iterate(mapping);
	}

private:
	// Strips the entity from all its components.
	void clear(Entity entity);
	
	// Returns the correct entity storage for the given type.
	template <typename T>
	EntityStorage* storage()
	{
		auto type = std::type_index(typeid(T));
		if (mapping.count(type) == 0) {
			mapping[type] = std::make_unique<EntityStorage>();
		}
		return mapping[type].get();
	}

public:
	using Storage = std::unique_ptr<EntityStorage>;
	using Mapping = std::unordered_map<std::type_index, Storage>;
	Mapping mapping;
};

/**
 * Global ECS until I rewrote the rest of the engine.
 */
inline ECS_t* ECS = []{ return new ECS_t; }();

}
