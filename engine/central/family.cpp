#include "engine/central/archetype.h"
#include "engine/central/entity.h"
#include "engine/central/family.h"


namespace kodanuki
{

Family::Family(Entity entity, Entity parent) noexcept
{
	this->itself = entity;
	this->parent = parent;
	
	if (parent) {
		ECS::get<Family>(parent).children.insert(entity);
	}
}

void Family::set_parent(Entity parent) noexcept
{
	this->parent = parent;
}

Entity Family::get_root() const noexcept
{
	return parent ? ECS::get<Family>(parent).get_root() : itself;
}

Entity Family::get_parent() const noexcept
{
	return parent;
}

std::unordered_set<Entity> Family::get_siblings() const noexcept
{
	if (!parent) {
		return {};
	}
	Family& parent_family = ECS::get<Family>(parent);
	std::unordered_set<Entity> siblings = parent_family.children;
	siblings.erase(itself);
	return siblings;
}

std::unordered_set<Entity> Family::get_children() const noexcept
{
	return children;
}

}
