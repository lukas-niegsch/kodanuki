#pragma once
#include "plugin/vulkan/device.h"
#include "plugin/vulkan/swapchain.h"
#include "plugin/vulkan/renderpass.h"
#include "plugin/vulkan/pipeline.h"
#include <glm/glm.hpp>
using namespace Kodanuki;

struct Vertex
{
    glm::vec2 pos;
    glm::vec3 color;

	static VkVertexInputBindingDescription binding_description()
	{
		VkVertexInputBindingDescription info = {};
		info.binding = 0;
		info.stride = sizeof(Vertex);
		info.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return info;
	}

	static std::array<VkVertexInputAttributeDescription, 2> attribute_descriptions()
	{
		std::array<VkVertexInputAttributeDescription, 2> infos = {};
		infos[0].binding = 0;
		infos[0].location = 0;
		infos[0].format = VK_FORMAT_R32G32_SFLOAT;
		infos[0].offset = offsetof(Vertex, pos);
		infos[1].binding = 0;
		infos[1].location = 1;
		infos[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		infos[1].offset = offsetof(Vertex, pos);
		return infos;
	}
};

// Creates the renderpass that describes the rendering of the example triangle.
VulkanRenderpass get_example_triangle_renderpass(VulkanDevice device);

// Creates the pipeline that renders the example triangle.
VulkanPipeline get_example_triangle_pipeline(VulkanDevice device, VulkanSwapchain swapchain, VulkanRenderpass renderpass);
