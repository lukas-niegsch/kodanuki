#pragma once
#include <vulkan/vulkan.h>
#include <memory>
#include <functional>
class GLFWwindow;

namespace kodanuki
{

/**
 * Shared wrapper around some given vulkan type.
 *
 * Each wrapper basically behaves the same way as a shared_ptr but it
 * also convertes to the base type. This just makes it easier to use
 * them in methods without having to dereference it. Also allows the
 * user to specify some custom destroy method.
 */
template <typename T>
struct Wrapper
{
public:
	/**
	 * Default constructor so this class plays nicely with std::shared_ptr.
	 */
	Wrapper() = default;

	/**
	 * Creates a new wrapper around the pointer.
	 *
	 * This wrapper now owns the pointer. It will destroy the pointer once
	 * this class is no longer used.
	 *
	 * @param ptr The pointer which will be wrapped.
	 * @param destroy The custom destroy method.
	 */
	Wrapper(T* ptr, std::function<void(T*)> destroy)
	{
		this->state = std::shared_ptr<T>(ptr, destroy);
	}

	/**
	 * Dereferences and returns the underlying type.
	 */
	operator T&()
	{
		return *state;
	}

	/**
	 * Dereferences and returns the underlying type.
	 */
	operator const T&() const
	{
		return *state;
	}

private:
	std::shared_ptr<T> state;
};

using VulkanInstance = Wrapper<VkInstance>;
using VulkanDevice = Wrapper<VkDevice>;
using VulkanShaderModule = Wrapper<VkShaderModule>;
using VulkanFence = Wrapper<VkFence>;
using VulkanSemaphore = Wrapper<VkSemaphore>;
using VulkanDescriptorPool = Wrapper<VkDescriptorPool>;
using VulkanDescriptorSet = Wrapper<VkDescriptorSet>;
using VulkanCommandPool = Wrapper<VkCommandPool>;
using VulkanCommandBuffer = Wrapper<VkCommandBuffer>;
using VulkanQueryPool = Wrapper<VkQueryPool>;
using VulkanFrameBuffer = Wrapper<VkFramebuffer>;
using VulkanImageView = Wrapper<VkImageView>;
using VulkanDeviceMemory = Wrapper<VkDeviceMemory>;
using VulkanSurface = Wrapper<VkSurfaceKHR>;
using VulkanSwapchain = Wrapper<VkSwapchainKHR>;
using VulkanPipeline = Wrapper<VkPipeline>;
using VulkanBuffer = Wrapper<VkBuffer>;

/**
 * Vulkan instances hold the context of the application.
 * 
 * @param layers The layers that should be enabled.
 * @param extensions The extensions that should be used.
 * @return The wrapper around the vulkan instance.
 */
VulkanInstance create_instance(std::vector<const char*> layers, std::vector<const char*> extensions);

/**
 * Devices are logical wrappers around physical graphics cards.
 *
 * @param physical_device The physical device that will be used.
 * @param queue_family The index of the used queue family.
 * @param queue_priorities The priorities and number of queues.
 * @param extensions The extensions that should be used.
 * @return The wrapper around the vulkan device.
 */
VulkanDevice create_device(VkPhysicalDevice physical_device, uint32_t queue_family, std::vector<float> queue_priorities, std::vector<const char*> extensions);

/**
 * Shader modules contain shader code that the device can execute.
 *
 * @param device The device that stores the shader.
 * @param code The bytes of the shader SPIRV shader file.
 * @return The wrapper around the vulkan shader module.
 */
VulkanShaderModule create_shader_module(VkDevice device, std::vector<char> code);

/**
 * Fences are synchronization primitives used for waiting inside the host.
 *
 * @param device The device that stores the fence.
 * @param flags The flags with which the fence is created.
 * @return The wrapper around the vulkan fence.
 */
VulkanFence create_fence(VkDevice device, VkFenceCreateFlagBits flags);

/**
 * Semaphores are synchronization primitives used for waiting inside the device.
 *
 * @param device The device that stores the semaphore.
 * @return The wrapper around the vulkan semaphore.
 */
VulkanSemaphore create_semaphore(VkDevice device);

/**
 * Descriptor pools maintain multiple descriptor sets.
 *
 * @param device The device that stores the descriptor pool.
 * @param pool_sizes The sizes of the descriptor pool for each type.
 * @return The wrapper around the vulkan descriptor pool.
 */
VulkanDescriptorPool create_descriptor_pool(VkDevice device, const std::vector<VkDescriptorPoolSize> pool_sizes);

/**
 * Descriptor sets hold the information for pipelines.
 *
 * @param device The device that stores the descriptor set.
 * @param pool The pool which maintains the descriptor set.
 * @param layout The layout with which the descriptor set is created.
 * @return The wrapper around the vulkan descriptor set.
 */
VulkanDescriptorSet create_descriptor_set(VkDevice device, VkDescriptorPool pool, const VkDescriptorSetLayout layout);

/**
 * Command pools maintain multiple command buffers.
 *
 * @param device The device that stores the command pool.
 * @param queue_family_index The queue family on which the command buffers operate.
 * @return The wrapper around the vulkan command pool.
 */
VulkanCommandPool create_command_pool(VkDevice device, uint32_t queue_family_index);

/**
 * Command buffers hold the information for queue commands.
 *
 * @param device The device that stores the command buffer.
 * @param pool The pool which maintains the command buffer.
 * @return The wrapper around the vulkan command buffer.
 */
VulkanCommandBuffer create_command_buffer(VkDevice device, VkCommandPool pool);

/**
 * Query pools maintain multiple timestamps.
 *
 * @param device The device that stores the query pool.
 * @param time_stamps The maximum number of timestamps.
 * @return The wrapper around the vulkan query pool.
 */
VulkanQueryPool create_query_pool(VkDevice device, uint32_t time_stamps);

/**
 * Framebuffers are the outputs from renderpasses.
 *
 * @param device The device that stores the frame buffer.
 * @param renderpass The renderpass which will produce frames.
 * @param extent The extent of the surface.
 * @param attachments The attachments that the renderpass produces.
 * @return The wrapper around the vulkan frame buffer.
 */
VulkanFrameBuffer create_frame_buffer(VkDevice device, VkRenderPass renderpass, VkExtent2D extent, std::vector<VkImageView> attachments);

/**
 * Image views allows for modifying vulkan images.
 *
 * @param device The device that stores the image view.
 * @param format The format how the image is interpreted.
 * @param image The image which should be modified.
 * @return The wrapper around the vulkan image view.
 */
VulkanImageView create_image_view(VkDevice device, VkFormat format, VkImage image, VkImageAspectFlagBits mask);

/**
 * Device memory maintains a blocks of memory.
 *
 * @param device The device that stores the device memory.
 * @param physical_device The device where the memory is allocated.
 * @param requirements The requirements that the memory must satisfy.
 * @param properties The properties that the memory must satisfy.
 * @return The wrapper around the vulkan device memory.
 */
VulkanDeviceMemory create_device_memory(VkDevice device, VkPhysicalDevice physical_device, VkMemoryRequirements requirements, VkMemoryPropertyFlags properties);

/**
 * Surfaces represent the renderable part of the screen.
 *
 * @param instance The instance that stores the context.
 * @param window The GLFW window on which frames are rendered.
 * @return The wrapper around the vulkan surface.
 */
VulkanSurface create_surface(VkInstance instance, GLFWwindow* window);

/**
 * Swapchains replace the different frames.
 *vkEndCommandBuffer
 * @param device The device that stores the swapchain.
 * @param info The information on how to create the swapchain.
 * @return The wrapper around the vulkan swapchain.
 */
VulkanSwapchain create_swapchain(VkDevice device, VkSwapchainCreateInfoKHR info);

/**
 * Compute pipelines allows running a series of commands.
 *
 * @param device The devies that stores the pipeline.
 * @param info The information on how to create the copute pipeline.
 * @return The wrapper around the vulkan pipeline.
 */
VulkanPipeline create_pipeline(VkDevice device, VkComputePipelineCreateInfo info);

/**
 * Graphics pipelines allows running some renderpass.
 *
 * @param device The devies that stores the pipeline.
 * @param info The information on how to create the copute pipeline.
 * @return The wrapper around the vulkan pipeline.
 */
VulkanPipeline create_pipeline(VkDevice device, VkGraphicsPipelineCreateInfo info);

/**
 * Buffers describe how some memory block is used.
 *
 * @param device The device that stores the buffer.
 * @param size The size of memory that the buffer contains.
 * @param usage The usage flags for what the buffer is used.
 * @return The wrapper around the vulkan buffer.
 */
VulkanBuffer create_buffer(VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage);

}
