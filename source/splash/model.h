#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>

namespace kodanuki
{

struct Vertex
{
	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 normal;
	glm::vec2 uv;

	bool operator==(const Vertex& other) const;
};

struct Instance
{
	glm::vec3 position;
};

struct MVP
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
};

struct Model
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
};

struct UD
{
	float delta_time;
	float kernel_size;
	float stiffness;
	float rho_0;
	float gravity;
	float viscosity;
	uint32_t particle_count;
};

// Loads the model from the given obj file.
Model load_obj_model(std::string filename);

}
