#pragma once
#include "engine/display/vkinit.h"
#include "engine/display/vkdraw.h"
#include "engine/display/vkmath.h"
#include <glm/glm.hpp>
using namespace kodanuki;


struct Vertex
{
	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 normal;
	glm::vec2 uv;

	bool operator==(const Vertex& other) const;
};

VulkanDevice create_device();
VulkanWindow create_window(VulkanDevice device, int width, int height);
VulkanTarget create_target(VulkanDevice device, VulkanWindow window);

vkdraw::fn_draw create_example_triangle(VulkanDevice device, VulkanTarget target);
