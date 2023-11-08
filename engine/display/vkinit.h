#pragma once
#include "engine/nekolib/templates/shared_wrapper.h"
#include "engine/nekolib/templates/optional_wrapper.h"
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vk_mem_alloc.h>
#include <SFML/Window.hpp>
#include <vector>


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
using memory_t            = shared_wrapper_t<VkDeviceMemory>;
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
};

struct VulkanWindow
{
	vktype::window_t                      window;
	vktype::surface_t                     surface;
	vktype::swapchain_t                   swapchain;
	vktype::img_specs_t                   image_specs;
	std::vector<vktype::image_view_t>     render_image_views;
	std::vector<vktype::command_buffer_t> render_buffers;
	std::vector<vktype::semaphore_t>      image_available_semaphores;
	std::vector<vktype::semaphore_t>      render_finished_semaphores;
	std::vector<vktype::fence_t>          aquire_frame_fences;
	vktype::image_t                       depth_image;
	vktype::image_view_t                  depth_image_view;
	vktype::memory_t                      depth_image_memory;
	uint32_t                              submit_frame;
	uint32_t                              render_frame;
};

struct VulkanTarget
{

};

struct VulkanTensor
{

};

struct VulkanShader
{

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

}

}
