#pragma once
#include "engine/utility/signature.h"
#include "engine/vulkan/debug.h"
#include <vulkan/vulkan.h>
#include <functional>
#include <memory>
class GLFWwindow;

namespace kodanuki
{

/**
 * Vectorizes properties that can be enumerated using vulkan.
 * 
 * This essentially only is a wrapper around vulkan's enumerate
 * pattern. Instead of having to calling these functions twice
 * (first to get the count, then to get the values), we return
 * a vector containing all the values instead.
 * 
 * usage:
 * vectorize<vkEnumerateInstanceExtensionProperties>(nullptr);
 */
template <auto Function, typename ... Args>
auto vectorize(Args ... args)
{
	using T = std::remove_pointer_t<reverse_signature_t<0, Function>>;
	uint32_t size;
	Function(args... , &size, nullptr);
	std::vector<T> result(size);
	Function(args... , &size, result.data());
	return result;
}

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
	operator T() const
	{
		if (state) {
			return *state;
		}
		return VK_NULL_HANDLE;
	}

private:
	std::shared_ptr<T> state;
};

/**
 * Creates a wrapper around vulkan's create/destroy pattern.
 *
 * The resulting wrapper will call the create method with the given
 * arguments and will automatically call the destroy method once
 * the object is no longer used.
 *
 * usage:
 * create_wrapper<vkCreateBuffer, vkDestroyBuffer>(...);
 */
template <auto CreateFunction, auto DestroyFunction, typename ... Args>
auto create_wrapper(std::remove_pointer_t<reverse_signature_t<2, CreateFunction>> arg0, Args ... args)
{
	using T = std::remove_pointer_t<reverse_signature_t<0, CreateFunction>>;
	T* output = new T();
	CHECK_VULKAN(CreateFunction(args..., &arg0, nullptr, output));
	auto destroy = [=](T* ptr) {
		DestroyFunction(args..., *ptr, nullptr);
	};
	return Wrapper<T>(output, destroy);
}

/**
 * Vulkan instances hold the context of the application.
 * 
 * @param layers The layers that should be enabled.
 * @param extensions The extensions that should be used.
 * @return The wrapper around the vulkan instance.
 */
Wrapper<VkInstance> create_instance(std::vector<const char*> layers, std::vector<const char*> extensions);

/**
 * Devices are logical wrappers around physical graphics cards.
 *
 * @param physical_device The physical device that will be used.
 * @param queue_family The index of the used queue family.
 * @param queue_priorities The priorities and number of queues.
 * @param extensions The extensions that should be used.
 * @return The wrapper around the vulkan device.
 */
Wrapper<VkDevice> create_device(VkPhysicalDevice physical_device, uint32_t queue_family, std::vector<float> queue_priorities, std::vector<const char*> extensions);

/**
 * Shader modules contain shader code that the device can execute.
 *
 * @param device The device that stores the shader.
 * @param code The bytes of the shader SPIRV shader file.
 * @return The wrapper around the vulkan shader module.
 */
Wrapper<VkShaderModule> create_shader_module(VkDevice device, std::vector<char> code);

/**
 * Fences are synchronization primitives used for waiting inside the host.
 *
 * @param device The device that stores the fence.
 * @param flags The flags with which the fence is created.
 * @return The wrapper around the vulkan fence.
 */
Wrapper<VkFence> create_fence(VkDevice device, VkFenceCreateFlagBits flags);

/**
 * Semaphores are synchronization primitives used for waiting inside the device.
 *
 * @param device The device that stores the semaphore.
 * @return The wrapper around the vulkan semaphore.
 */
Wrapper<VkSemaphore> create_semaphore(VkDevice device);

/**
 * Descriptor pools maintain multiple descriptor sets.
 *
 * @param device The device that stores the descriptor pool.
 * @param pool_sizes The sizes of the descriptor pool for each type.
 * @return The wrapper around the vulkan descriptor pool.
 */
Wrapper<VkDescriptorPool> create_descriptor_pool(VkDevice device, const std::vector<VkDescriptorPoolSize> pool_sizes);

/**
 * Descriptor sets hold the information for pipelines.
 *
 * @param device The device that stores the descriptor set.
 * @param pool The pool which maintains the descriptor set.
 * @param layout The layout with which the descriptor set is created.
 * @return The wrapper around the vulkan descriptor set.
 */
Wrapper<VkDescriptorSet> create_descriptor_set(VkDevice device, VkDescriptorPool pool, const VkDescriptorSetLayout layout);

/**
 * Command pools maintain multiple command buffers.
 *
 * @param device The device that stores the command pool.
 * @param queue_family_index The queue family on which the command buffers operate.
 * @return The wrapper around the vulkan command pool.
 */
Wrapper<VkCommandPool> create_command_pool(VkDevice device, uint32_t queue_family_index);

/**
 * Command buffers hold the information for queue commands.
 *
 * @param device The device that stores the command buffer.
 * @param pool The pool which maintains the command buffer.
 * @return The wrapper around the vulkan command buffer.
 */
Wrapper<VkCommandBuffer> create_command_buffer(VkDevice device, VkCommandPool pool);

/**
 * Query pools maintain multiple timestamps.
 *
 * @param device The device that stores the query pool.
 * @param time_stamps The maximum number of timestamps.
 * @return The wrapper around the vulkan query pool.
 */
Wrapper<VkQueryPool> create_query_pool(VkDevice device, uint32_t time_stamps);

/**
 * Framebuffers are the outputs from renderpasses.
 *
 * @param device The device that stores the frame buffer.
 * @param renderpass The renderpass which will produce frames.
 * @param extent The extent of the surface.
 * @param attachments The attachments that the renderpass produces.
 * @return The wrapper around the vulkan frame buffer.
 */
Wrapper<VkFramebuffer> create_frame_buffer(VkDevice device, VkRenderPass renderpass, VkExtent2D extent, std::vector<VkImageView> attachments);

/**
 * Image views allows for modifying vulkan images.
 *
 * @param device The device that stores the image view.
 * @param format The format how the image is interpreted.
 * @param image The image which should be modified.
 * @return The wrapper around the vulkan image view.
 */
Wrapper<VkImageView> create_image_view(VkDevice device, VkFormat format, VkImage image, VkImageAspectFlagBits mask);

/**
 * Device memory maintains a blocks of memory.
 *
 * @param device The device that stores the device memory.
 * @param physical_device The device where the memory is allocated.
 * @param requirements The requirements that the memory must satisfy.
 * @param properties The properties that the memory must satisfy.
 * @return The wrapper around the vulkan device memory.
 */
Wrapper<VkDeviceMemory> create_device_memory(VkDevice device, VkPhysicalDevice physical_device, VkMemoryRequirements requirements, VkMemoryPropertyFlags properties);

/**
 * Surfaces represent the renderable part of the screen.
 *
 * @param instance The instance that stores the context.
 * @param window The GLFW window on which frames are rendered.
 * @return The wrapper around the vulkan surface.
 */
Wrapper<VkSurfaceKHR> create_surface(VkInstance instance, GLFWwindow* window);

/**
 * Swapchains replace the different frames.
 *vkEndCommandBuffer
 * @param device The device that stores the swapchain.
 * @param info The information on how to create the swapchain.
 * @return The wrapper around the vulkan swapchain.
 */
Wrapper<VkSwapchainKHR> create_swapchain(VkDevice device, VkSwapchainCreateInfoKHR info);

/**
 * Compute pipelines allows running a series of commands.
 *
 * @param device The devies that stores the pipeline.
 * @param info The information on how to create the copute pipeline.
 * @return The wrapper around the vulkan pipeline.
 */
Wrapper<VkPipeline> create_pipeline(VkDevice device, VkComputePipelineCreateInfo info);

/**
 * Graphics pipelines allows running some renderpass.
 *
 * @param device The devies that stores the pipeline.
 * @param info The information on how to create the copute pipeline.
 * @return The wrapper around the vulkan pipeline.
 */
Wrapper<VkPipeline> create_pipeline(VkDevice device, VkGraphicsPipelineCreateInfo info);

/**
 * Buffers describe how some memory block is used.
 *
 * @param device The device that stores the buffer.
 * @param size The size of memory that the buffer contains.
 * @param usage The usage flags for what the buffer is used.
 * @return The wrapper around the vulkan buffer.
 */
Wrapper<VkBuffer> create_buffer(VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage);

/**
 * Renderpasses describe the vulkan graphics pipelines.
 *
 * @param device The device that stores the renderpass.
 * @param builder The functon that creates the renderpass.
 * @return The wrapper around the vulkan renderpass.
 */
Wrapper<VkRenderPass> create_renderpass(VkDevice device, std::function<void(VkDevice, VkRenderPass&)> builder);

/**
 * Images hold multidimension drawing data.
 *
 * @param device The device that stores the image.
 * @param info The information on how to create the image.
 * @return The wrapper around the vulkan image.
 */
Wrapper<VkImage> create_image(VkDevice device, VkImageCreateInfo info);

}
