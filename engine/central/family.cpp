#include "engine/central/archetype.h"
#include "engine/central/entity.h"
#include "engine/central/family.h"
#include <cassert>

namespace kodanuki {

void ExitLocalFamily(Family& family, Entity oldParent)
{
	family.parent = {};
	Family& parentFamily = ECS::get<Family>(oldParent);
	parentFamily.children.erase(family.itself);

	for (Entity sibling : family.siblings)
	{
		Family& siblingFamily = ECS::get<Family>(sibling);
		siblingFamily.siblings.erase(family.itself);
	}

	family.siblings.clear();
}

void EnterLocalFamily(Family& family, Entity newParent)
{
	family.parent = newParent;
	Family& parentFamily = ECS::get<Family>(newParent);
	
	for (Entity sibling : parentFamily.children)
	{
		Family& siblingFamily = ECS::get<Family>(sibling);
		siblingFamily.siblings.insert(family.itself);
		family.siblings.insert(sibling);
	}

	parentFamily.children.insert(family.itself);
}

void UpdateRootRecursive(Family& family, Entity newRoot)
{
	family.root = newRoot;

	for (Entity child : family.children)
	{
		Family& childFamily = ECS::get<Family>(child);
		UpdateRootRecursive(childFamily, newRoot);
	}
}

// GCC Bugzilla #104606: remove pragmas once the bug is fixed
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
void UpdateRoot(Family& family, Entity oldParent, Entity newParent)
{	
	Entity oldRoot = oldParent ? ECS::get<Family>(oldParent).root : std::nullopt;
	Entity newRoot = newParent ? ECS::get<Family>(newParent).root : std::nullopt;
	
	if (oldRoot == newRoot)
	{
		return;
	}

	UpdateRootRecursive(family, newRoot ? newRoot : family.itself);
}
#pragma GCC diagnostic pop

void UpdateLocalFamily(Entity entity, Entity newParent)
{
	Family& family = ECS::get<Family>(entity);
	Entity oldParent = family.parent;

	if (oldParent == newParent)
	{
		return;
	}

	if (oldParent)
	{
		ExitLocalFamily(family, oldParent);
	}

	if (newParent)
	{
		EnterLocalFamily(family, newParent);
	}

	UpdateRoot(family, oldParent, newParent);
}

void UpdateDefaultFamily(Entity entity)
{
	ECS::update<Family>(entity, {});
	ECS::get<Family>(entity).itself = entity;
	ECS::get<Family>(entity).root = entity;
}

void update_family(Entity entity, Entity newParent)
{
	assert(entity);
	assert(!newParent || ECS::has<Family>(newParent));

	if (!ECS::has<Family>(entity))
	{
		UpdateDefaultFamily(entity);
	}

	UpdateLocalFamily(entity, newParent);
}

}
