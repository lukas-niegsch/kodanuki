#include "engine/central/entity.h"
#include "engine/central/family.h"
#include "engine/utility/counter.h"

namespace Kodanuki
{

Entity ECS::create(Entity parent)
{
	uint64_t id = count();
	Entity entity = std::make_optional<uint64_t>(id);
	ECS::update<Entity>(entity, entity);
	update_family(entity, parent);
	return entity;
}

}
