#pragma once
#include "source/splash/scene.h"
#include "source/splash/model.h"
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
	VulkanPipeline update_pipeline_pressure;
	VulkanPipeline update_pipeline_simulate;
};

struct SimulateTensors
{
	VulkanTensor tensor_index;
	VulkanTensor tensor_vertex;
	VulkanTensor tensor_mass;
	VulkanTensor tensor_mvp;
	VulkanTensor tensor_ud;
	VulkanTensor tensor_position;
	VulkanTensor tensor_velocity;
	VulkanTensor tensor_pressure;
	VulkanTensor tensor_density;
};

class ShaderBridge
{
public:
	ShaderBridge(ShaderBridgeBuilder builder);
	~ShaderBridge();
	uint32_t get_index_count();
	uint32_t get_instance_count();
	void bind_render_resources(VkCommandBuffer buffer, uint32_t frame);
	void bind_update_pressure_resources(VkCommandBuffer buffer, uint32_t frame);
	void bind_update_simulate_resources(VkCommandBuffer buffer, uint32_t frame);

	void update_mvp(const MVP& new_mvp, uint32_t frame);
	void update_ud(const UD& new_ud, uint32_t frame);

private:
	void create_render_descriptors();
	void create_update_pressure_descriptors();
	void create_update_simulate_descriptors();
	void update_descriptor(VkDescriptorSet descriptor, uint32_t binding, VkDescriptorType type, VulkanTensor tensor, uint32_t frame);

	void load_tensor_data(Model model, Scene scene);
	SimulateTensors create_simulation_tensors(ShaderBridgeBuilder builder);

private:
	uint32_t count_frame;
	uint32_t count_index;
	uint32_t count_vertex;
	uint32_t count_instance;
	VkPipelineLayout render_pipeline_layout;
	VkPipelineLayout update_pressure_pipeline_layout;
	VkPipelineLayout update_simulate_pipeline_layout;
	VkDescriptorSetLayout render_descriptor_layout;
	VkDescriptorSetLayout update_pressure_descriptor_layout;
	VkDescriptorSetLayout update_simulate_descriptor_layout;
	VkDescriptorPool descriptor_pool;
	VulkanDevice device;
	std::vector<VkDescriptorSet> render_descriptors;
	std::vector<VkDescriptorSet> update_pressure_descriptors;
	std::vector<VkDescriptorSet> update_simulate_descriptors;
	VulkanPipelineCache cache;
	SimulateTensors tensors;
};

}
