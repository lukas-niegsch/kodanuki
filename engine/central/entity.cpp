#include "engine/central/entity.h"
#include "engine/utility/counter.h"

namespace Kodanuki
{

Entity ECS::create()
{
	uint64_t id = count();
	return std::make_optional<uint64_t>(id);
}

Mapping ECS::mapping;

}
