#include "engine/central/entity.h"
#include "engine/central/family.h"
#include "engine/central/utility/counter.h"

namespace kodanuki
{

Entity ECS::create(Entity parent)
{
	Entity entity = std::make_optional<uint64_t>(count());
	ECS::update<Entity>(entity, entity);
	ECS::update<Family>(entity, {entity, parent});
	return entity;
}

void ECS::clear(Entity entity, bool initial)
{
	if (!ECS::has<Entity>(entity)) {
		return;
	}
	Family& family = ECS::get<Family>(entity);
	if (initial) {
		family.set_parent(std::nullopt);
	}
	for (Entity child : family.get_children()) {
		ECS::clear(child, false);
	}
	mapping.remove(entity.value());
}

}
