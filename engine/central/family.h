#pragma once
#include "engine/central/entity.h"
#include <unordered_set>

namespace kodanuki
{

/**
 * The family stores the local family sub-tree.
 *
 * The family tree is uniquely defined by the parent relationship.
 * You can update the parent of the given entity and all other family
 * members are calculated automatically. Optionally, lazy evaluation
 * can be enabled.
 *
 * The family component should not be removed.
 */
class Family
{
public:
	/**
	 * Creates the family component of the given entity.
	 *
	 * @param entity The entity at the center of the family sub-tree.
	 * @param parent The parent of the given entity.
	 */
	Family(Entity entity, Entity parent = {}) noexcept;

	/**
	 * Sets the parent of the given entity and updates the surrounding
	 * family tree.
	 *
	 * @param parent The parent of the given entity.
	 */
	void set_parent(Entity parent = {}) noexcept;

	/**
	 * Returns the root of the family tree.
	 *
	 * The root is either the entity itself or the root of the parent.
	 *
	 * @return The root of the family tree.
	 */
	Entity get_root() const noexcept;

	/**
	 * Returns the parent of the given entity.
	 *
	 * The parent may be invalid depending, see constructor.
	 *
	 * @return The parent of the given entity.
	 */
	Entity get_parent() const noexcept;

	/**
	 * Returns the siblings of the given entity.
	 *
	 * The siblings are all children of the parent except the
	 * entity itself.
	 *
	 * @return The siblings of the given entity.
	 */
	std::unordered_set<Entity> get_siblings() const noexcept;

	/**
	 * Returns the children of the given entity.
	 *
	 * @return The siblings of the given entity.
	 */
	std::unordered_set<Entity> get_children() const noexcept;

private:
	Entity itself;
	Entity parent;
	std::unordered_set<Entity> children;
};

} // namespace kodanuki
