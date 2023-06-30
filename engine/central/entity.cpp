#include "engine/central/entity.h"
#include "engine/central/family.h"
#include "engine/utility/counter.h"

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
	for (auto& pair : mapping) {
		pair.second->remove(entity.value());
	}
	update_family(entity, std::nullopt);
}

}
