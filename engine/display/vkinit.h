#pragma once
#include "engine/nekolib/templates/shared_wrapper.h"
#include "engine/nekolib/templates/optional_wrapper.h"
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <SFML/Window.hpp>
#include <vector>
#include <sstream>


/**
 * Makro that terminates the program and prints some debug information.
 */
#define ERROR(reason)									\
	do {												\
		std::stringstream err;							\
		err << "VkResult was " << (reason) << '\n';		\
		err << "[Error] in file: " << __FILE__ << '\n';	\
		err << "[Error] in line: " << __LINE__ << '\n';	\
		throw std::runtime_error(err.str());    		\
	} while (false)

/**
 * Makro that checks if the vulkan function call was executed successfully.
 */
#define CHECK_VULKAN(result)					\
	do {										\
		auto return_type = result;				\
		if (return_type != VK_SUCCESS) {		\
			ERROR(return_type);					\
		}										\
	} while (false)

/**
 * Prints the given result into the output stream.
 */
std::ostream& operator<<(std::ostream& os, const VkResult& result);


namespace kodanuki
{

namespace vktype
{

using instance_t          = shared_wrapper_t<VkInstance>;
using device_t            = shared_wrapper_t<VkDevice>;
using shader_module_t     = shared_wrapper_t<VkShaderModule>;
using fence_t             = shared_wrapper_t<VkFence>;
using semaphore_t         = shared_wrapper_t<VkSemaphore>;
using command_pool_t      = shared_wrapper_t<VkCommandPool>;
using command_buffer_t    = shared_wrapper_t<VkCommandBuffer>;
using renderpass_t        = shared_wrapper_t<VkRenderPass>;
using pipeline_t          = shared_wrapper_t<VkPipeline>;
using pipeline_layout_t   = shared_wrapper_t<VkPipelineLayout>;
using surface_t           = shared_wrapper_t<VkSurfaceKHR>;
using swapchain_t         = shared_wrapper_t<VkSwapchainKHR>;
using framebuffer_t       = shared_wrapper_t<VkFramebuffer>;
using buffer_t            = shared_wrapper_t<VkBuffer>;
using image_t             = shared_wrapper_t<VkImage>;
using image_view_t        = shared_wrapper_t<VkImageView>;
using descriptor_pool_t   = shared_wrapper_t<VkDescriptorPool>;
using descriptor_layout_t = shared_wrapper_t<VkDescriptorSetLayout>;
using descriptor_set_t    = shared_wrapper_t<VkDescriptorSet>;
using window_t            = shared_wrapper_t<sf::WindowBase>;
using vma_t               = shared_wrapper_t<VmaAllocator>;

struct hardware_t
{
	VkPhysicalDevice                 physical_device;
	VkPhysicalDeviceProperties       physical_device_properties;
	VkPhysicalDeviceFeatures         physical_device_features;
	VkPhysicalDeviceMemoryProperties physical_device_memory_properties;
	VkQueueFamilyProperties          queue_family_properties;
	uint32_t                         queue_family_index;
};

struct img_specs_t
{
	VkFormat         depth_format;
	VkFormat         color_format;
	VkColorSpaceKHR  color_space;
	VkPresentModeKHR present_mode;
	uint32_t         frame_count;
};

}


struct VulkanDevice
{
	vktype::instance_t        instance;
	vktype::hardware_t        hardware;
	vktype::device_t          device;
	vktype::command_pool_t    command_pool;
	vktype::descriptor_pool_t descriptor_pool;
	vktype::vma_t             allocator;

	operator VkDevice() const { return device; }
};

struct VulkanWindow
{
	vktype::window_t                      window;
	vktype::surface_t                     surface;
	vktype::swapchain_t                   swapchain;
	vktype::img_specs_t                   image_specs;
	std::vector<VkImage>                  render_images;
	std::vector<vktype::image_view_t>     render_image_views;
	std::vector<vktype::command_buffer_t> render_buffers;
	std::vector<vktype::semaphore_t>      image_available_semaphores;
	std::vector<vktype::semaphore_t>      render_finished_semaphores;
	std::vector<vktype::fence_t>          aquire_frame_fences;
	vktype::image_t                       depth_image;
	vktype::image_view_t                  depth_image_view;
	VkExtent2D                            surface_extent;
	uint32_t                              submit_frame;
	uint32_t                              render_frame;

	void recreate(VulkanDevice device);
};

struct VulkanTarget
{
	vktype::descriptor_layout_t descriptor_layout;
	vktype::descriptor_set_t    descriptor_set;
	vktype::pipeline_layout_t   pipeline_layout;
	vktype::pipeline_t          graphics_pipeline;
};

struct VulkanTensor
{
	vktype::buffer_t         primary_buffer;
	vktype::buffer_t         staging_buffer;
	void*                    staging_memory;
	std::vector<std::size_t> shape;
	uint32_t                 element_size;
	uint32_t                 element_count;

	template <typename T>
	T& get(std::vector<std::size_t> indices)
	{
		return *(static_cast<T*>(staging_memory) + offset(indices));
	}
	std::size_t offset(std::vector<std::size_t> indices);
};


namespace vkinit
{

struct VulkanDeviceBuilder
{
	std::vector<const char*>               instance_layers;
	std::vector<const char*>               instance_extensions;
	std::vector<const char*>               device_layers;
	std::vector<const char*>               device_extensions;
	std::function<int(vktype::hardware_t)> score_device;
	std::vector<float>                     queue_priorities;
};
OptionalWrapper<VulkanDevice> device(const VulkanDeviceBuilder& builder);


struct VulkanWindowBuilder
{
	std::string         title;
	std::pair<int, int> shape;
	VkFormat            depth_format;
	VkFormat            color_format;
	VkColorSpaceKHR     color_space;
	VkPresentModeKHR    present_mode;
	uint32_t            frame_count;
};
OptionalWrapper<VulkanWindow> window(const VulkanWindowBuilder& builder, VulkanDevice device);


struct VulkanTargetBuilder
{
	std::string                                    path_vertex_shader;
	std::string                                    path_fragment_shader;
	std::vector<VkPushConstantRange>               push_constants;
	std::vector<VkDescriptorSetLayoutBinding>      descriptor_bindings;
	std::vector<VkVertexInputBindingDescription>   vertex_input_bindings;
	std::vector<VkVertexInputAttributeDescription> vertex_input_attributes;
	VkPrimitiveTopology                            vertex_input_topology;
};
OptionalWrapper<VulkanTarget> target(const VulkanTargetBuilder& builder, VulkanDevice device, VulkanWindow window);


struct VulkanTensorBuilder
{
	std::vector<std::size_t> shape;
	uint32_t                 element_size;
	VkBufferUsageFlags       usage;
};
OptionalWrapper<VulkanTensor> tensor(const VulkanTensorBuilder& builder, VulkanDevice device);

}

}
