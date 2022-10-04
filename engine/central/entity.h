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
class ECS_t
{
public:
	// Creates a new entity with a unique identifier.
	Entity create();

	// Updates the given component inside the entity.
	template <typename T>
	void update(Entity entity, T component = {});

	// Removes the given component from the entity.
	template <typename T>
	void remove(Entity entity);

	// Returns true iff the entity has the given component.
	template <typename T>
	bool has(Entity entity);

	// Returns the reference to the component.
	template <typename T>
	T& get(Entity entity);

	// Copies the component from the source entity to the target entity.
	template <typename T>
	void copy(Entity source, Entity target);

	// Moves the component from the source entity to the target entity.
	template <typename T>
	void move(Entity source, Entity target);

	// Swaps the component from the source entity with the target entity.
	template <typename T>
	void swap(Entity source, Entity target);

	// Binds the component from the source entity to the target entity.
	template <typename T>
	void bind(Entity source, Entity target);

	// Iterates over entities with the given archetype.
	template <typename Archetype>
	auto iterate();

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

#include "engine/central/entity.tpp"

