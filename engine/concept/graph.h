#pragma once
#include "engine/central/entity.h"
#include <set>

namespace Kodanuki
{

struct Graph
{
	std::set<Entity> neighbours;
};

}
