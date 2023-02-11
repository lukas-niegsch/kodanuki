#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>

namespace kodanuki
{

struct Scene
{
	uint32_t instance_count;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> velocities;
	std::vector<float> masses;
};

// Loads the scene from the given csv file.
Scene load_csv_scene(std::string filename);

}
