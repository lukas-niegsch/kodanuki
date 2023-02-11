#pragma once
#include <glm/vec3.hpp>

namespace kodanuki
{

/**
 * Defines the location of an entity in world space coordinates.
 */
struct Location
{
	glm::vec3 position;
	glm::vec3 direction;
};

}

