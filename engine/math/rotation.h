#pragma once
#include <glm/vec3.hpp>
#include <tuple>

namespace Kodanuki
{

/**
 * Calculates the rotation vectors in xyz direction from the given euler
 * angles in radians.
 */
std::tuple<glm::vec3, glm::vec3, glm::vec3> euler_rotate(glm::vec3 rotation);

}

