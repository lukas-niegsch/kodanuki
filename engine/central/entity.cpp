#include "engine/central/entity.h"
#include "engine/central/family.h"
#include "engine/central/utility/counter.h"

namespace kodanuki
{

Entity ECS::create(Entity parent)
{
	uint64_t id = count();
	Entity entity = std::make_optional<uint64_t>(id);
	ECS::update<Entity>(entity, entity);
	update_family(entity, parent);
	return entity;
}

void ECS::clear(Entity entity)
{
	update_family(entity, std::nullopt);
	std::set<Entity> children = ECS::get<Family>(entity).children;
	for (Entity child : children) {
		ECS::clear(child);
	}
	mapping.remove(entity.value());
}

}
