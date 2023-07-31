#pragma once
#include "engine/central/storage.h"
#include <cstdint>
#include <optional>
#include <any>

namespace kodanuki
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
class ECS
{
public:
	/**
	 * Creates a new entity with a unique identifier.
	 *
	 * @param parent The potential parent for this entity.
	 */
	static Entity create(Entity parent = std::nullopt);

	// Updates the given component inside the entity.
	template <typename T>
	static void update(Entity entity, T component = {})
	{
		mapping.get<T>().update(entity.value(), component);
	}

	// Removes the given component from the entity.
	template <typename T>
	static void remove(Entity entity)
	{
		if constexpr (std::is_same<T, Entity>()) {
			clear(entity);
		} else {
			mapping.get<T>().remove(entity.value());
		}
	}

	// Returns true iff the entity has the given component.
	template <typename T>
	static bool has(Entity entity)
	{
		return mapping.get<T>().contains(entity.value());
	}

	// Returns the reference to the component.
	template <typename T>
	static T& get(Entity entity)
	{
		return mapping.get<T>()[entity.value()];
	}

	// Copies the component from the source entity to the target entity.
	template <typename T>
	static void copy(Entity source, Entity target)
	{
		if (!has<T>(source)) {
			return;
		}
		update<T>(target, get<T>(source));
	}

	// Moves the component from the source entity to the target entity.
	template <typename T>
	static void move(Entity source, Entity target)
	{
		if (!has<T>(source)) {
			return;
		}
		copy<T>(source, target);
		remove<T>(source);
	}

	// Swaps the component from the source entity with the target entity.
	template <typename T>
	static void swap(Entity source, Entity target)
	{
		if (!has<T>(source)) {
			move<T>(target, source);
			return;
		}
		if (!has<T>(target)) {
			move<T>(source, target);
			return;
		}
		auto source_value = get<T>(source);
		auto target_value = get<T>(target);
		update<T>(target, source_value);
		update<T>(source, target_value);
	}

	// Binds the component from the source entity to the target entity.
	template <typename T>
	static void bind(Entity source, Entity target)
	{
		mapping.get<T>().bind(source.value(), target.value());
	}

	// Iterates over entities with the given archetype.
	template <typename Archetype>
	static auto iterate()
	{
		return Archetype::iterate(mapping);
	}

private:
	// Strips the entity from all its components.
	static void clear(Entity entity);

private:
	static inline EntityMapping mapping;
};

}
