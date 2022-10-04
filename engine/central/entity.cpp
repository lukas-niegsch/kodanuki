#include "engine/central/entity.h"
#include "engine/utility/counter.h"

namespace Kodanuki
{

Entity ECS_t::create()
{
	uint64_t id = count();
	Entity entity = std::make_optional<uint64_t>(id);
	ECS->update<Entity>(entity, entity);
	return entity;
}

}
