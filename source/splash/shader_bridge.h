#pragma once
#include "source/splash/scene.h"
#include "source/splash/model.h"
#include "engine/utility/alignment.h"
#include "engine/vulkan/device.h"
#include "engine/vulkan/memory.h"
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

/**
 * Memory layout:
 * once:
 * - indices
 * - vertices
 * - instance mass
 * - instance pressure
 * - instance density
 *
 * per frame:
 * - MPV
 * - UD
 * - instance position
 * - instance velocity
 */
class ShaderBridge
{
public:
	ShaderBridge(ShaderBridgeBuilder builder);
	~ShaderBridge();
	VulkanMemory::StagingBuffer<align<MVP, 256>> get_buffer_mvp(uint32_t frame);
	VulkanMemory::StagingBuffer<align<UD, 256>> get_buffer_ud(uint32_t frame);
	uint32_t get_index_count();
	uint32_t get_instance_count();
	void bind_render_resources(VkCommandBuffer buffer, uint32_t frame);
	void bind_update_pressure_resources(VkCommandBuffer buffer, uint32_t frame);
	void bind_update_simulate_resources(VkCommandBuffer buffer, uint32_t frame);

	void update_mvp(const MVP& new_mvp, uint32_t frame);
	void update_ud(const UD& new_ud, uint32_t frame);

private:
	VulkanMemory allocate_memory(Model model, Scene scene);
	void create_render_descriptors();
	void create_update_pressure_descriptors();
	void create_update_simulate_descriptors();
	void update_descriptor(VkDescriptorSet descriptor, uint32_t binding, VkDescriptorType type, VkDeviceSize offset, VkDeviceSize size);
	void print_memory_layout();

	void load_tensor_data(Model model, Scene scene);

private:
	uint32_t count_frame;
	uint32_t count_index;
	uint32_t count_vertex;
	uint32_t count_instance;
	uint32_t size_index;
	uint32_t size_vertex;
	uint32_t size_mass;
	uint32_t size_pressure;
	uint32_t size_density;
	uint32_t size_mvp;
	uint32_t size_ud;
	uint32_t size_position;
	uint32_t size_velocity;
	uint32_t offset_indices;
	uint32_t offset_vertices;
	uint32_t offset_mass;
	uint32_t offset_pressure;
	uint32_t offset_density;
	uint32_t offset_total;
	uint32_t offset_frame_mvp;
	uint32_t offset_frame_ud;
	uint32_t offset_frame_position;
	uint32_t offset_frame_velocity;
	uint32_t offset_frame_total;
	VkPipelineLayout render_pipeline_layout;
	VkPipelineLayout update_pressure_pipeline_layout;
	VkPipelineLayout update_simulate_pipeline_layout;
	VkDescriptorSetLayout render_descriptor_layout;
	VkDescriptorSetLayout update_pressure_descriptor_layout;
	VkDescriptorSetLayout update_simulate_descriptor_layout;
	VkDescriptorPool descriptor_pool;
	VulkanDevice device;
	VulkanMemory memory;
	VkBuffer memory_buffer;
	std::vector<VkDescriptorSet> render_descriptors;
	std::vector<VkDescriptorSet> update_pressure_descriptors;
	std::vector<VkDescriptorSet> update_simulate_descriptors;

	VulkanPipelineCache cache;
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

}
