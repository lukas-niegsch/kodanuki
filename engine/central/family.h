#pragma once
#include "engine/central/entity.h"

namespace kodanuki
{

/**
 * The family stores the calculated family tree.
 */
struct Family
{
	// The current root entity of this family.
	Entity root;
	// The current parent of this entity.
	Entity parent;
	// The current entity itself.
	Entity itself;
	// The current siblings of this entity.
	std::set<Entity> siblings;
	// The current children of this entity.
	std::set<Entity> children;
};

/**
 * Updates the family of the given entity.
 * 
 * The family tree is uniquely defined by the parent relationship. You can
 * update the parent of the given entity and all other family members are
 * calculated automatically.
 * 
 * This function assumes that the parent has a family component. It will
 * add/update the family component of the entity. These changes might also
 * update the surrounding family tree.
 * 
 * @param entity The entity for which the family should change.
 * @param parent The new parent for the given entity.
 */
void update_family(Entity entity, Entity parent = std::nullopt);

} // namespace kodanuki
