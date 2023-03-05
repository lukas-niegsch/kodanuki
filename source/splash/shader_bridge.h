#pragma once
#include "source/splash/scene.h"
#include "source/splash/model.h"
#include "source/splash/simulation.h"
#include "engine/utility/alignment.h"
#include "engine/vulkan/device.h"
#include "engine/vulkan/pipeline.h"
#include "engine/vulkan/tensor.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace kodanuki
{

struct ShaderBridgeBuilder
{
	VulkanDevice device;
	uint32_t frame_count;
	Model model;
	Scene scene;
	VulkanPipeline render_pipeline;
};

struct RenderTensors
{
	VulkanTensor tensor_index;
	VulkanTensor tensor_vertex;
	VulkanTensor tensor_mvp;
};

class ShaderBridge
{
public:
	ShaderBridge(ShaderBridgeBuilder builder);
	~ShaderBridge();
	uint32_t get_index_count();
	uint32_t get_instance_count();
	void bind_render_resources(VkCommandBuffer buffer, uint32_t frame);
	void update_mvp(const MVP& new_mvp, uint32_t frame);
	void tick_simulation(uint32_t frame, float delta_time);

private:
	void create_render_descriptors();
	RenderTensors create_render_tensors(ShaderBridgeBuilder builder);
	void load_tensor_data(Model model);

private:
	uint32_t count_frame;
	uint32_t count_index;
	uint32_t count_vertex;
	VkPipelineLayout render_pipeline_layout;
	VkDescriptorSetLayout render_descriptor_layout;
	VkDescriptorPool descriptor_pool;
	VulkanDevice device;
	std::vector<VkDescriptorSet> render_descriptors;
	RenderTensors tensors;
	Simulation simulation;
	VulkanPipelineCache cache;
};

}
