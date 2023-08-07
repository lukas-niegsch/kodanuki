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
	 * Each entity stores the following components:
	 *     - Entity
	 *     - Family
	 *
	 * @param parent The potential parent for this entity.
	 */
	static Entity create(Entity parent = std::nullopt);

	/**
	 * Updates the component inside the entity.
	 *
	 * @param T The type of the component.
	 * @param entity The entity to update the component.
	 * @param component The value of the component.
	 */
	template <typename T>
	static void update(Entity entity, T component = {})
	{
		mapping.get<T>().update(entity.value(), component);
	}

	/**
	 * Removes the given component from the entity.
	 *
	 * Removing the Entity component will stip the entity of all
	 * its components (including the entity one). Clearing also
	 * removes the components of all of its children.
	 *
	 * This function should not be called with the Family component.
	 *
	 * @param T The type of the component.
	 * @param entity The entity to remove the component.
	 */
	template <typename T>
	static void remove(Entity entity)
	{
		if constexpr (std::is_same<T, Entity>()) {
			clear(entity);
		} else {
			mapping.get<T>().remove(entity.value());
		}
	}

	/**
	 * Returns true iff the entity has the component.
	 *
	 * @param T The type of the component.
	 * @param entity The entity to check for the component.
	 * @return Does the entity contain the component.
	 */
	template <typename T>
	static bool has(Entity entity)
	{
		return mapping.get<T>().contains(entity.value());
	}

	/**
	 * Returns the reference to the component.
	 *
	 * @param T The type of the component.
	 * @param enttiy The entity to get the component.
	 * @return The reference to the component.
	 */
	template <typename T>
	static T& get(Entity entity)
	{
		return mapping.get<T>()[entity.value()];
	}

	/**
	 * Copies the component from the source entity to the target entity.
	 * Does nothing if the source entity doesn't have this component.
	 *
	 * @param T The type of the component.
	 * @param source The entity that contains the component.
	 * @param target The entity to which to copy the component.
	 */
	template <typename T>
	static void copy(Entity source, Entity target)
	{
		if (!has<T>(source)) {
			return;
		}
		update<T>(target, get<T>(source));
	}

	/**
	 * Moves the component from the source entity to the target entity.
	 * Does nothing if the target entity doesn't have this component.
	 *
	 * @param T The type of the component.
	 * @param source The entity that contains the component.
	 * @param target The entity to which to move the component.
	 */
	template <typename T>
	static void move(Entity source, Entity target)
	{
		if (!has<T>(source)) {
			return;
		}
		copy<T>(source, target);
		remove<T>(source);
	}

	/**
	 * Swaps the component from the source entity with the target entity.
	 * Does nothing if neither entities have this component. If only one
	 * of them has it, then the component will be moved instead.
	 *
	 * @param T The type of the component.
	 * @param source The entity containing the source component.
	 * @param target The entity containing the target component.
	 */
	template <typename T>
	static void swap(Entity source, Entity target)
	{
		if (has<T>(source) && has<T>(target)) {
			std::swap(get<T>(source), get<T>(target));
			return;
		}
		move<T>(target, source);
		move<T>(source, target);
	}

	/**
	 * Binds the component from the source entity to the target entity.
	 *
	 * Bound components share the same memory. That means updating the
	 * component will affect both the source and target entities. When
	 * removing these components, they will only be completely deleted
	 * once the last entity containing it has removed it.
	 *
	 * If the target does not contain the component, this will remove
	 * the source component if present.
	 *
	 * @param T The type of the component.
	 * @param source The entity that contains the component.
	 * @param target The entity to which to bind the component.
	 */
	template <typename T>
	static void bind(Entity source, Entity target)
	{
		mapping.get<T>().bind(source.value(), target.value());
	}

	/**
	 * Iterates over entities with the given archetype.
	 *
	 * @param Archetype The archetype that defines the iteration.
	 * @return An iterator over tuples of components.
	 */
	template <typename Archetype>
	static auto iterate()
	{
		return Archetype::iterate(mapping);
	}

private:
	// Strips the entity from all its components.
	static void clear(Entity entity, bool initial = true);

private:
	static inline EntityMapping mapping;
};

}
