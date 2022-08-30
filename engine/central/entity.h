#pragma once
#include "engine/central/storage.h"
#include <cstdint>
#include <optional>

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
class ECS
{
public:
	// Creates a new entity with a unique identifier.
	static Entity create();

	// Updates the given component inside the entity.
	template <typename T>
	static void update(Entity entity, T component = {});

	// Removes the given component from the entity.
	template <typename T>
	static void remove(Entity entity);

	// Removes the given component from each entity.
	template <typename T>
	static void remove();

	// Returns true iff the entity has the given component.
	template <typename T>
	static bool has(Entity entity);

	// Returns true iff any entity has the given component.
	template <typename T>
	static bool has();

	// Returns the reference to the component.
	template <typename T>
	static T& get(Entity entity);

	// Copies the component from the source entity to the target entity.
	template <typename T>
	static void copy(Entity source, Entity target);

	// Copies all components from the source entity to the target entity.
	template <typename A, typename B, typename ... T>
	static void copy(Entity source, Entity target);

	// Moves the component from the source entity to the target entity.
	template <typename T>
	static void move(Entity source, Entity target);

	// Moves all components from the source entity to the target entity.
	template <typename A, typename B, typename ... T>
	static void move(Entity source, Entity target);

	// Swaps the component from the source entity with the target entity.
	template <typename T>
	static void swap(Entity source, Entity target);

	// Swaps all components from the source entity with the target entity.
	template <typename A, typename B, typename ... T>
	static void swap(Entity source, Entity target);

	// Binds the component from the source entity to the target entity.
	template <typename T>
	static void bind(Entity source, Entity target, bool weak = false);

	// Binds all components from the source entity to the target entity.
	template <typename A, typename B, typename ... T>
	static void bind(Entity source, Entity target, bool weak = false);

	// Iterates over entities with the given archetype.
	template <typename Archetype>
	static auto iterate(bool weak = false);

private:
	using Storage = std::unique_ptr<EntityStorage>;
	using Mapping = std::unordered_map<std::type_index, Storage>;
	static inline Mapping mapping;
};

}

#include "engine/central/entity.tpp"
