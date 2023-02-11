#include "engine/math/rotation.h"
#include <glm/trigonometric.hpp>

namespace kodanuki
{

std::tuple<glm::vec3, glm::vec3, glm::vec3> euler_rotate(glm::vec3 rotation)
{
	float cx = glm::cos(rotation.x);
  	float sx = glm::sin(rotation.x);
	float cy = glm::cos(rotation.y);
  	float sy = glm::sin(rotation.y);
	float cz = glm::cos(rotation.z);
	float sz = glm::sin(rotation.z);

	glm::vec3 rx = {cy * cz, - cy * sz, sy};
	glm::vec3 ry = {sx * sy * cz + cx * sz, cx * cz - sx * sy * sz, - sx * cy};
	glm::vec3 rz = {sx * sz - cx * sy * cz, cx * sy * sz + sx * cz, cx * cy};
	return {rx, ry, rz};
}

}

