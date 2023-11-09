#define VMA_IMPLEMENTATION
#include "engine/display/vkinit.h"
#include "engine/nekolib/algorithm/alignment.h"
#include "engine/nekolib/parsing/file.h"
#include "engine/nekolib/templates/signature.h"
#include "engine/nekolib/templates/type_name.h"
#include <sstream>
#include <stdexcept>


namespace kodanuki::vkinit
{

/**
 * Enables debug printing inside the autowrapper method.
 */
static constexpr bool ENABLE_AUTOWRAPPER_PRINTING = false; 

/**
 * The vulkan API version that is used inside the renderer.
 */
static constexpr uint32_t VULKAN_VERSION = VK_API_VERSION_1_3;

template <auto fn_create>
constexpr bool is_vma = [](){
	using T = reverse_signature_t<0, fn_create>;
	return std::is_same_v<T, VmaAllocationInfo*>; 
}();

template <auto fn_create>
constexpr bool is_allocate = [](){
	using T = reverse_signature_t<1, fn_create>;
	return !std::is_same_v<T, const VkAllocationCallbacks*>;
}();

/**
 * The underlying vulkan type for the wrapper.
 *
 * Example:
 * vkCreateDevice    -> VkDevice
 * vkCreateSemaphore -> VkSemaphore
 */
template <auto fn_create>
using vulkan_t = std::remove_pointer_t<reverse_signature_t<0 + 2 * is_vma<fn_create>, fn_create>>;

/**
 * The create info type for the create method.
 *
 * Example:
 * vkCreateDevice    -> VkDeviceCreateInfo
 * vkCreateSemaphore -> VkSemaphoreCreateInfo
 */
template <auto fn_create>
using config_t = std::remove_const_t<std::remove_pointer_t<reverse_signature_t<2 - is_allocate<fn_create> + 3 * is_vma<fn_create>, fn_create>>>;

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
 *
 * @param args The optional arguments for the passed function.
 */
template <auto fn_enumerate, typename ... Args>
auto vectorize(Args ... args)
{
	using T = vulkan_t<fn_enumerate>;
	uint32_t size;
	fn_enumerate(args... , &size, nullptr);
	std::vector<T> result(size);
	fn_enumerate(args... , &size, result.data());
	return result;
}

/**
 * Creates a wrapper around vulkan's create/destroy pattern.
 *
 * The resulting wrapper will call the create method with the given
 * arguments and will automatically call the destroy method once
 * the object is no longer used.
 *
 * Also works with allocate/free but only for one object at once.
 *
 * usage:
 * autocreate<vkCreateBuffer, vkDestroyBuffer>(...);
 */
template <auto fn_create, auto fn_delete, typename ... args_t>
auto autowrapper(const config_t<fn_create>& config, args_t ... args)
{
    using T = vulkan_t<fn_create>;
    constexpr bool is_allocate_wrapper = sizeof...(args_t) == 2;

	if constexpr (ENABLE_AUTOWRAPPER_PRINTING) {
		std::cout << "[INFO] create: " << type_name<T>() << std::endl;
	}
	T* output = new T();
	VmaAllocation* allocation = nullptr;

	if constexpr (is_vma<fn_create>) {
		VmaAllocationCreateInfo alloc_info = {};
		alloc_info.usage = VMA_MEMORY_USAGE_AUTO;
		allocation = new VmaAllocation;
		CHECK_VULKAN(fn_create(args..., &config, &alloc_info, output, allocation, nullptr));
	} else if constexpr (std::is_same_v<T, VmaAllocator>) {
		CHECK_VULKAN(fn_create(args..., &config, output));
	} else if constexpr (std::is_same_v<T, VkPipeline>) {
        CHECK_VULKAN(fn_create(args..., VK_NULL_HANDLE, 1, &config, nullptr, output));
    } else if constexpr (is_allocate_wrapper) {
        auto[device, _] = std::tie(args...);
        CHECK_VULKAN(fn_create(device, &config, output));
    } else {
        CHECK_VULKAN(fn_create(args..., &config, nullptr, output));
    }

    auto destroy = [=](T* ptr) {
		if constexpr (ENABLE_AUTOWRAPPER_PRINTING) {
        	std::cout << "[INFO] delete: " << type_name<T>() << std::endl;
		}
		if constexpr (is_vma<fn_create>) {
			fn_delete(args..., *ptr, *allocation);
			delete allocation;
		} else if constexpr (std::is_same_v<T, VmaAllocator>) {
			fn_delete(args..., *ptr);
		} else if constexpr (std::is_same_v<T, VkPipeline>) {
            fn_delete(args..., *ptr, nullptr);
        } else if constexpr (std::is_same_v<T, VkDevice>) {
            fn_delete(*ptr, nullptr);
        } else if constexpr (is_allocate_wrapper) {
            fn_delete(args..., 1, ptr);
        } else {
            fn_delete(args..., *ptr, nullptr);
        }
        delete ptr;
    };
	return shared_wrapper_t<T>(output, destroy);
}

/**
 * The vulkan memory allocator from AMD does memory management. Instead of
 * calling vkAllocateMemory, you just create the objects directory with
 * special wrapper methods.
 *
 * @param instance The instance holding the vulkan context.
 * @param hardware The configuration of the physical device.
 * @param device The used logical device.
 */
vktype::vma_t create_vulkan_memory_allocator(
	vktype::instance_t instance,
	vktype::hardware_t hardware,
	vktype::device_t   device)
{
	return autowrapper<vmaCreateAllocator, vmaDestroyAllocator>({
		.flags = 0,
 		.physicalDevice = hardware.physical_device,
		.device = device,
		.preferredLargeHeapBlockSize = 0,
 		.pAllocationCallbacks = nullptr,
		.pDeviceMemoryCallbacks = nullptr,
		.pHeapSizeLimit = nullptr,
		.pVulkanFunctions = nullptr,
 		.instance = instance,
		.vulkanApiVersion = VULKAN_VERSION,
		.pTypeExternalMemoryHandleTypes = nullptr,
	});
}

/**
 * Vulkan instances hold the context of the application.
 * 
 * @param layers The layers that should be enabled.
 * @param extensions The extensions that should be used.
 */
vktype::instance_t create_instance(
	std::vector<const char*> layers,
	std::vector<const char*> extensions)
{
	VkApplicationInfo app_info = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pNext = nullptr,
		.pApplicationName = nullptr,
		.applicationVersion = 0,
		.pEngineName = "kodanuki",
		.engineVersion = VK_MAKE_VERSION(0, 0, 1),
		.apiVersion = VULKAN_VERSION,
	};
	return autowrapper<vkCreateInstance, vkDestroyInstance>({
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.pApplicationInfo = &app_info,
		.enabledLayerCount = static_cast<uint32_t>(layers.size()),
		.ppEnabledLayerNames = layers.data(),
		.enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
		.ppEnabledExtensionNames = extensions.data()
	});
}

/**
 * Returns the vector of all different hardwares.
 * 
 * There will be one for each physical devices and queue family
 * pair. This way there might be multiple entries for the same
 * physical device. The specs contains the properties and features
 * of the physical device and the queue family.
 *
 * @param instance The instance for which to enumerate the devices.
 */
std::vector<vktype::hardware_t> query_physical_devices(VkInstance instance)
{
	std::vector<vktype::hardware_t> devices;
	
	for (auto device : vectorize<vkEnumeratePhysicalDevices>(instance))
	{
		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(device, &properties);

		VkPhysicalDeviceFeatures features;
		vkGetPhysicalDeviceFeatures(device, &features);

		VkPhysicalDeviceMemoryProperties memory_properties;
		vkGetPhysicalDeviceMemoryProperties(device, &memory_properties);

		uint32_t queue_family_index = 0;
		for (auto queue : vectorize<vkGetPhysicalDeviceQueueFamilyProperties>(device))
		{	
			vktype::hardware_t hardware = {
				device, properties, features, memory_properties,
				queue, queue_family_index};

			devices.push_back(hardware);
			queue_family_index++;
		}
	}

	return devices;
}

/**
 * Returns the best scoring physical device.
 *
 * Calls the scoring function for the physical devices once and then
 * picks the best scoring one. The best scoring one will be the one
 * with the highest int value. In case of ties one of them will be
 * picked (which one is undefined).
 *
 * @param instance The instance for which to enumerate the devices.
 * @param fn_score The scoring method that ranks the physical devices.
 */
vktype::hardware_t select_physical_device(VkInstance instance, std::function<int(vktype::hardware_t)> fn_score)
{
	std::vector<vktype::hardware_t> devices = query_physical_devices(instance);
	return *std::ranges::max_element(devices, std::ranges::less{}, fn_score);
}

/**
 * Vulkan devices are the logical representation of the GPU.
 *
 * @param specs The specs of the physical device.
 * @param extensions The extensions that the device should use.
 * @param queue_priorities One priority for each created queue.
 */
vktype::device_t create_device(
	vktype::hardware_t       hardware,
	std::vector<const char*> layers,
	std::vector<const char*> extensions,
	std::vector<float>       queue_priorities)
{
	VkDeviceQueueCreateInfo info = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.queueFamilyIndex = hardware.queue_family_index,
		.queueCount = static_cast<uint32_t>(queue_priorities.size()),
		.pQueuePriorities = queue_priorities.data(),
	};
	VkPhysicalDeviceVulkan13Features extra_features = {};
	extra_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
	extra_features.dynamicRendering = VK_TRUE;
	return autowrapper<vkCreateDevice, vkDestroyDevice>({
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pNext = &extra_features,
		.flags = 0,
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &info,
		.enabledLayerCount = static_cast<uint32_t>(layers.size()),
		.ppEnabledLayerNames = layers.data(),
		.enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
		.ppEnabledExtensionNames = extensions.data(),
		.pEnabledFeatures = &hardware.physical_device_features,
	}, hardware.physical_device);
}

/**
 * Vulkan command pools allocate command buffers.
 *
 * @param device The device that stores the command pool.
 * @param queue_family_index The index of the queue family.
 */
vktype::command_pool_t create_command_pool(
	vktype::device_t device,
	uint32_t         queue_family_index)
{
	return autowrapper<vkCreateCommandPool, vkDestroyCommandPool>({
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.pNext = nullptr,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = queue_family_index
	}, device);
}

/**
 * Vulkan command buffers stores commands submitted to queues.
 *
 * @param device The device that stores the command buffer.
 * @param pool The command pool that allocates the buffer.
 */
vktype::command_buffer_t create_command_buffer(
	vktype::device_t       device,
	vktype::command_pool_t pool)
{
	return autowrapper<vkAllocateCommandBuffers, vkFreeCommandBuffers>({
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = nullptr,
		.commandPool = pool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1
	}, device, pool);
}

/**
 * Vulkan descriptor pools allocate descriptor sets.
 *
 * @param device The device that stores the descriptor pool.
 * @param max_uniform_pool_size The descriptor pool sizes for each type.
 * @param max_uniform_set_size The number of allowed descriptor sets.
 */
vktype::descriptor_pool_t create_descriptor_pool(
	vktype::device_t device,
	uint32_t         max_uniform_pool_size = 30,
	uint32_t         max_uniform_set_size = 10)
{
	std::vector<VkDescriptorPoolSize> pool_sizes = {
		{ VK_DESCRIPTOR_TYPE_SAMPLER, max_uniform_pool_size },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, max_uniform_pool_size },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, max_uniform_pool_size },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, max_uniform_pool_size },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, max_uniform_pool_size },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, max_uniform_pool_size },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, max_uniform_pool_size },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, max_uniform_pool_size },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, max_uniform_pool_size },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, max_uniform_pool_size },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, max_uniform_pool_size }
	};
	uint32_t pool_sizes_count = pool_sizes.size();
	return autowrapper<vkCreateDescriptorPool, vkDestroyDescriptorPool>({
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.pNext = nullptr,
		.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
		.maxSets = max_uniform_set_size * pool_sizes_count,
		.poolSizeCount = pool_sizes_count,
		.pPoolSizes = pool_sizes.data()
	}, device);
}

/**
 * Vulkan descriptor sets store information about ressources bound during
 * the pipeline stages, e.g. uniform buffers.
 *
 * @param device The device that stores the descriptor set.
 * @param pool The descriptor pool from which the set is allocated.
 * @param layout The layout for the descriptor set.
 */
vktype::descriptor_set_t create_descriptor_set(
	vktype::device_t            device,
	vktype::descriptor_pool_t   pool,
	vktype::descriptor_layout_t layout)
{
	return autowrapper<vkAllocateDescriptorSets, vkFreeDescriptorSets>({
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = nullptr,
		.descriptorPool = pool,
		.descriptorSetCount = 1,
		.pSetLayouts = layout,
	}, device, pool);
}

/**
 * Vulkan swapchains replace the images for the some surface.
 *
 * @param device The device that stores the swapchain.
 * @param surface The surface on which to place the images.
 * @param specs The configuration for the images.
 */
vktype::swapchain_t create_swapchain(
	vktype::device_t          device,
	vktype::surface_t         surface,
	vktype::img_specs_t       specs,
	VkExtent2D                surface_extent,
	const vktype::swapchain_t old_swapchain = {})
{
	return autowrapper<vkCreateSwapchainKHR, vkDestroySwapchainKHR>({
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.pNext = nullptr,
		.flags = 0,
		.surface = surface,
		.minImageCount = specs.frame_count,
		.imageFormat = specs.color_format,
		.imageColorSpace = specs.color_space,
		.imageExtent = surface_extent,
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = nullptr,
		.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
		.compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
		.presentMode = specs.present_mode,
		.clipped = VK_TRUE,
		.oldSwapchain = old_swapchain,
	}, device);
}

/**
 * Vulkan surfaces describe the region on which the swapchain renders.
 *
 * The passed method only creates the surface. This method also handles
 * the convertion to the memory safe wrapper type.
 *
 * @param instance The vulkan instance for the create method.
 * @param fn_create_surface The method that creates the surface.
 */
vktype::surface_t create_surface(
	vktype::instance_t                             instance,
	std::function<void(VkInstance, VkSurfaceKHR&)> fn_create_surface)
{
	VkSurfaceKHR* surface = new VkSurfaceKHR();
	fn_create_surface(instance, *surface);
	auto deleter = [=](VkSurfaceKHR* ptr) {
		VkInstance casted_instance = instance;
		vkDestroySurfaceKHR(casted_instance, *ptr, nullptr);
		delete ptr;
	};
	return vktype::surface_t(surface, deleter);
}

/**
 * Vulkan image views describe the access and usage of an image.
 *
 * @param device The device that stores the image view.
 * @param format The format of the image.
 * @param image The actual image to view at.
 * @param mask The flag describing the purpose of the image.
 */
vktype::image_view_t create_image_view(
	vktype::device_t device,
	VkFormat format,
	VkImage image,
	VkImageAspectFlagBits mask)
{
	VkComponentMapping components = {
		.r = VK_COMPONENT_SWIZZLE_IDENTITY,
		.g = VK_COMPONENT_SWIZZLE_IDENTITY,
		.b = VK_COMPONENT_SWIZZLE_IDENTITY,
		.a = VK_COMPONENT_SWIZZLE_IDENTITY
	};
	VkImageSubresourceRange subresource_range = {
		.aspectMask = mask,
		.baseMipLevel = 0,
		.levelCount = 1,
		.baseArrayLayer = 0,
		.layerCount = 1
	};
	return autowrapper<vkCreateImageView, vkDestroyImageView>({
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.image = image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = format,
		.components = components,
		.subresourceRange = subresource_range
	}, device);
}

/**
 * Vulkan fences allows synchronization with the CPU.
 *
 * @param device The device that stores the device.
 * @param flags The creation flags for this fence.
 */
vktype::fence_t create_fence(
	vktype::device_t      device,
	VkFenceCreateFlagBits flags)
{
	return autowrapper<vkCreateFence, vkDestroyFence>({
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.pNext = nullptr,
		.flags = flags
	}, device);
}

/**
 * Vulkan fences allows synchronization with the GPU.
 *
 * @param device The device that stores the device.
 */
vktype::semaphore_t create_semaphore(
	vktype::device_t device)
{
	return autowrapper<vkCreateSemaphore, vkDestroySemaphore>({
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0
	}, device);
}

/**
 * Vulkan descriptor set layout describe how descriptor sets can be bound
 * during the different pipeline stages.
 *
 * @param device The device that stores the layouts.
 * @param bindings The binding information for the layouts.
 */
vktype::descriptor_layout_t create_descriptor_layout(
	vktype::device_t                          device,
	std::vector<VkDescriptorSetLayoutBinding> bindings)
{
	return autowrapper<vkCreateDescriptorSetLayout, vkDestroyDescriptorSetLayout>({
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.bindingCount = static_cast<uint32_t>(bindings.size()),
		.pBindings = bindings.data(),
	}, device);
}

/**
 * Vulkan shader modules hold the SPIR-V shader code for compilation.
 *
 * @param device The device that stores the shader.
 * @param file_path The path to the SPIR-V file, e.g. example.frag.
 */
vktype::shader_module_t create_shader_module(
	vktype::device_t device,
	const char*      file_path)
{
	std::vector<char> code = read_file_into_buffer(file_path);
	return autowrapper<vkCreateShaderModule, vkDestroyShaderModule>({
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.codeSize = code.size(),
		.pCode = reinterpret_cast<const uint32_t*>(code.data())
	}, device);
}

/**
 * Vulkan images stores the actual texture data.
 *
 * @param allocator The vma allocator for that creates this image.
 * @param image_format The format of the image.
 * @param image_extent The suface extent in 2D of the image.
 * @param image_usage The usage bits for the image.
 */
vktype::image_t create_image(
	vktype::vma_t     allocator,
	VkFormat          image_format,
	VkExtent2D        image_extent,
	VkImageUsageFlags image_usage)
{
	return autowrapper<vmaCreateImage, vmaDestroyImage>({
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.imageType = VK_IMAGE_TYPE_2D,
		.format = image_format,
		.extent = {image_extent.width, image_extent.height, 1},
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.usage = image_usage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = nullptr,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
	}, allocator);
}

/**
 * Vulkan buffers store how memory they require and how they use it.
 *
 * @param allocator The vma allocator for that creates this buffer.
 * @param usage_flags The types of elements that can be stored.
 * @param element_size The required size of each element.
 * @param element_count The number of elements that can be stored.
 */
vktype::buffer_t create_buffer(
	vktype::vma_t      allocator,
	VkBufferUsageFlags usage_flags,
	uint32_t           element_size,
	uint32_t           element_count)
{
	return autowrapper<vmaCreateBuffer, vmaDestroyBuffer>({
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.size = align_modulo(element_size * element_count, 256),
		.usage = usage_flags,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = nullptr,
	}, allocator);
}

/**
 * Vulkan pipeline layout describe how ressources like descriptor sets can
 * be bound during the different stages.
 *
 * @param device The device that stores the pipeline layout.
 * @param layouts The descriptor set layouts that can be bound.
 * @param constants The push constants that can be bound. 
 */
vktype::pipeline_layout_t create_pipeline_layout(
	vktype::device_t                         device,
	std::vector<vktype::descriptor_layout_t> layouts,
	std::vector<VkPushConstantRange>         constants = {})
{
	std::vector<VkDescriptorSetLayout> native_layouts(layouts.begin(), layouts.end());
	return autowrapper<vkCreatePipelineLayout, vkDestroyPipelineLayout>({
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.setLayoutCount = static_cast<uint32_t>(native_layouts.size()),
		.pSetLayouts = native_layouts.data(),
		.pushConstantRangeCount = static_cast<uint32_t>(constants.size()),
		.pPushConstantRanges = constants.data(),
	}, device);
}

/**
 * Vulkan graphics pipeline consists of multiple stages that transforms
 * vertex data into images. It has fixed function stages and programmable
 * stages.
 *
 * We create a very simple pipeline here. It only has two programmable stages,
 * the vertex and fragment shaders. It renders the whole image, does depth
 * testing, simple color blending, no multisampling and no tesselation.
 *
 * @param device The device that stores and compiles the pipeline.
 * @param layout The layout of the pipeline.
 * @param vertex_shader The vertex shader.
 * @param fragment_shader The fragment shader.
 * @param viewport_extent The extent of the view, e.g. the whole window.
 * @param input_topology The input tology, e.g. triangle lists.
 * @param input_bindings The description on how often inputs are bound and their size.
 * @param input_attributes The description for the different binding attributes.
 */
vktype::pipeline_t create_graphics_pipeline(
	vktype::device_t                               device,
	vktype::pipeline_layout_t                      layout,
	vktype::shader_module_t                        vertex_shader,
	vktype::shader_module_t                        fragment_shader,
	VkExtent2D                                     viewport_extent,
	VkPrimitiveTopology                            input_topology,
	std::vector<VkVertexInputBindingDescription>   input_bindings,
	std::vector<VkVertexInputAttributeDescription> input_attributes,
	VkFormat                                       color_format,
	VkFormat                                       depth_format)
{
	std::array<VkPipelineShaderStageCreateInfo, 2> shader_stages;
	for (auto& stage : shader_stages) {
    	stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    	stage.pNext = nullptr;
    	stage.flags = 0;
    	stage.pName = "main";
    	stage.pSpecializationInfo = nullptr;
	}
	shader_stages[0].module = vertex_shader;
	shader_stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	shader_stages[1].module = fragment_shader;
	shader_stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	VkPipelineVertexInputStateCreateInfo vertex_input = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.vertexBindingDescriptionCount = static_cast<uint32_t>(input_bindings.size()),
		.pVertexBindingDescriptions = input_bindings.data(),
		.vertexAttributeDescriptionCount = static_cast<uint32_t>(input_attributes.size()),
		.pVertexAttributeDescriptions = input_attributes.data(),
	};
	VkPipelineInputAssemblyStateCreateInfo input_assembly = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.topology = input_topology,
		.primitiveRestartEnable = VK_FALSE,
	};
	VkViewport keep_everything_viewport = {
		.x = 0.0f,
		.y = 0.0f,
		.width = static_cast<float>(viewport_extent.width),
		.height = static_cast<float>(viewport_extent.height),
		.minDepth = 0.0f,
		.maxDepth = 1.0f,
	};
	VkRect2D keep_everything_scissor = {
		.offset = {0, 0},
		.extent = viewport_extent,
	};
	VkPipelineViewportStateCreateInfo viewport = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.viewportCount = 1,
		.pViewports = &keep_everything_viewport,
		.scissorCount = 1,
		.pScissors = &keep_everything_scissor,
	};
	VkPipelineRasterizationStateCreateInfo resterization = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.depthClampEnable = VK_FALSE,
		.rasterizerDiscardEnable = VK_FALSE,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.cullMode = VK_CULL_MODE_BACK_BIT,
		.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
		.depthBiasEnable = VK_FALSE,
		.depthBiasConstantFactor = 0.0f,
		.depthBiasClamp = 0.0f,
		.depthBiasSlopeFactor = 0.0f,
		.lineWidth = 1.0f,
	};
	VkPipelineMultisampleStateCreateInfo multisample = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
		.sampleShadingEnable = VK_FALSE,
		.minSampleShading = 1.0f,
		.pSampleMask = nullptr,
		.alphaToCoverageEnable = VK_FALSE,
		.alphaToOneEnable = VK_FALSE,
	};
	VkPipelineDepthStencilStateCreateInfo depth_stencil = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.depthTestEnable = VK_TRUE,
		.depthWriteEnable = VK_TRUE,
		.depthCompareOp = VK_COMPARE_OP_LESS,
		.depthBoundsTestEnable = VK_FALSE,
		.stencilTestEnable = VK_FALSE,
		.front = {},
		.back = {},
		.minDepthBounds = 0.0f,
		.maxDepthBounds = 1.0f,		
	};
	VkPipelineColorBlendAttachmentState color_blend_attachment = {
		.blendEnable = VK_TRUE,
		.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
		.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
		.colorBlendOp = VK_BLEND_OP_ADD,
		.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
		.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
		.alphaBlendOp = VK_BLEND_OP_ADD,
		.colorWriteMask = VK_COLOR_COMPONENT_A_BIT | VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT,
	};
	VkPipelineColorBlendStateCreateInfo color_blend = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.logicOpEnable = VK_FALSE,
		.logicOp = VK_LOGIC_OP_NO_OP,
		.attachmentCount = 1,
		.pAttachments = &color_blend_attachment,
		.blendConstants = {0.0f, 0.0f, 0.0f, 0.0f},
	};
	VkPipelineRenderingCreateInfo dynamic_rendering_info = {
    	.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
    	.pNext = nullptr,
    	.viewMask = 0,
    	.colorAttachmentCount = 1,
    	.pColorAttachmentFormats = &color_format,
    	.depthAttachmentFormat = depth_format,
    	.stencilAttachmentFormat = {},
	};
	return autowrapper<vkCreateGraphicsPipelines, vkDestroyPipeline>({
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext = &dynamic_rendering_info,
		.flags = 0,
		.stageCount = shader_stages.size(),
		.pStages = shader_stages.data(),
		.pVertexInputState = &vertex_input,
		.pInputAssemblyState = &input_assembly,
		.pTessellationState = nullptr,
		.pViewportState = &viewport,
		.pRasterizationState = &resterization,
		.pMultisampleState = &multisample,
		.pDepthStencilState = &depth_stencil,
		.pColorBlendState = &color_blend,
		.pDynamicState = nullptr,
		.layout = layout,
		.renderPass = VK_NULL_HANDLE,
		.subpass = 0,
		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = -1,
	}, device);
}

/**
 * Returns the current surface extent.
 *
 * @param hardware The properties of the physical device.
 * @param surface The surface of the window.
 */
VkExtent2D get_surface_extent(vktype::hardware_t hardware, vktype::surface_t surface)
{
	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
		hardware.physical_device, surface, &capabilities);
	return capabilities.currentExtent;
}

OptionalWrapper<VulkanDevice> device(const VulkanDeviceBuilder& builder)
{
	VulkanDevice device;

	try {
		device.instance = create_instance(
			builder.instance_layers, builder.instance_extensions);
	} catch (const std::runtime_error& error) {
		return {{}, error.what(), "Failed to create instance."};
	}

	try {
		device.hardware = select_physical_device(device.instance,
			builder.score_device);
	} catch (const std::runtime_error& error) {
		return {{}, error.what(), "Failed to select physical device."};
	}

	try {
		device.device = create_device(device.hardware, builder.device_layers,
			builder.device_extensions, builder.queue_priorities);
	} catch (const std::runtime_error& error) {
		return {{}, error.what(), "Failed to create logical device."};
	}

	try {
		device.command_pool = create_command_pool(device.device,
			device.hardware.queue_family_index);
	} catch (const std::runtime_error& error) {
		return {{}, error.what(), "Failed to create command pool."};
	}

	try {
		device.descriptor_pool = create_descriptor_pool(device.device);
	} catch (const std::runtime_error& error) {
		return {{}, error.what(), "Failed to create descriptor pool."};
	}

	try {
		device.allocator = create_vulkan_memory_allocator(
			device.instance, device.hardware, device.device);
	} catch (const std::runtime_error& error) {
		return {{}, error.what(), "Failed to create vulkan memory allocator."};
	}

	return {device, "", ""};
}

OptionalWrapper<VulkanWindow> finalize_dynamic_window(VulkanWindow window, VulkanDevice device)
{
	window.surface_extent = get_surface_extent(device.hardware, window.surface);
	window.submit_frame = 0;
	window.render_frame = 0;

	try {
		window.swapchain = create_swapchain(device.device,
			window.surface, window.image_specs, window.surface_extent,
			window.swapchain);
	} catch (std::runtime_error& error) {
		return {{}, error.what(), "Failed to create swapchain."};
	}

	try {
		window.render_images = vectorize<vkGetSwapchainImagesKHR>(
			device.device, window.swapchain);
		for (uint32_t i = 0; i < window.image_specs.frame_count; i++) {
			window.render_image_views[i] = create_image_view(
				device.device, window.image_specs.color_format,
				window.render_images[i], VK_IMAGE_ASPECT_COLOR_BIT);
		}
	} catch (std::runtime_error& error) {
		return {{}, error.what(), "Failed to create render image views."};
	}

	try {
		window.depth_image = create_image(device.allocator,
			window.image_specs.depth_format, window.surface_extent,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
	} catch (std::runtime_error& error) {
		return {{}, error.what(), "Failed to create depth image."};
	}

	try {
		window.depth_image_view = create_image_view(device.device,
			window.image_specs.depth_format, window.depth_image,
			VK_IMAGE_ASPECT_DEPTH_BIT);
	} catch (std::runtime_error& error) {
		return {{}, error.what(), "Failed to create depth image view."};
	}

	return {window, "", ""};
}

OptionalWrapper<VulkanWindow> window(const VulkanWindowBuilder& builder, VulkanDevice device)
{
	VulkanWindow window;
	window.image_specs.depth_format = builder.depth_format;
	window.image_specs.color_format = builder.color_format;
	window.image_specs.color_space = builder.color_space;
	window.image_specs.present_mode = builder.present_mode;
	window.image_specs.frame_count = builder.frame_count;
	window.render_image_views.resize(window.image_specs.frame_count);
	window.render_buffers.resize(window.image_specs.frame_count);
	window.image_available_semaphores.resize(window.image_specs.frame_count);
	window.render_finished_semaphores.resize(window.image_specs.frame_count);
	window.aquire_frame_fences.resize(window.image_specs.frame_count);

	try {
		window.window = vktype::window_t(new sf::WindowBase(sf::VideoMode(
			builder.shape.first, builder.shape.second), builder.title),
			[](sf::WindowBase* ptr) {delete ptr; });
	} catch (std::runtime_error& error) {
		return {{}, error.what(), "Failed to create native window."};
	}

	try {
		window.surface = create_surface(device.instance,
			[&](VkInstance instance, VkSurfaceKHR& surface) {
				static_cast<sf::WindowBase&>(window.window).createVulkanSurface(
					instance, surface); });
	} catch (std::runtime_error& error) {
		return {{}, error.what(), "Failed to create surface."};
	}

	try {
		for (uint32_t i = 0; i < window.image_specs.frame_count; i++) {
			window.render_buffers[i] = create_command_buffer(
				device.device, device.command_pool);
		}
	} catch (std::runtime_error& error) {
		return {{}, error.what(), "Failed to create command buffers."};
	}

	try {
		for (uint32_t i = 0; i < window.image_specs.frame_count; i++) {
			window.image_available_semaphores[i] = create_semaphore(device.device);
			window.render_finished_semaphores[i] = create_semaphore(device.device);
			window.aquire_frame_fences[i] = create_fence(device.device, VK_FENCE_CREATE_SIGNALED_BIT);
		}
	} catch (std::runtime_error& error) {
		return {{}, error.what(), "Failed to create synchronization objects."};
	}

	return finalize_dynamic_window(window, device);
}

OptionalWrapper<VulkanTarget> target(const VulkanTargetBuilder& builder, VulkanDevice device, VulkanWindow window)
{
	VulkanTarget target;

	try {
		target.descriptor_layout = create_descriptor_layout(
			device.device, builder.descriptor_bindings);
	} catch (std::runtime_error& error) {
		return {{}, error.what(), "Failed to create descriptor layout."};
	}

	try {
		target.descriptor_set = create_descriptor_set(
			device.device, device.descriptor_pool, target.descriptor_layout);
	} catch (std::runtime_error& error) {
		return {{}, error.what(), "Failed to create descriptor set."};
	}

	try {
		target.pipeline_layout = create_pipeline_layout(
			device.device, {target.descriptor_layout}, builder.push_constants);
	} catch (std::runtime_error& error) {
		return {{}, error.what(), "Failed to create pipeline layout."};
	}

	try {
		vktype::shader_module_t vertex_shader = create_shader_module(
			device.device, builder.path_vertex_shader.c_str());

		vktype::shader_module_t fragment_shader = create_shader_module(
			device.device, builder.path_fragment_shader.c_str());

		target.graphics_pipeline = create_graphics_pipeline(
			device.device, target.pipeline_layout,
			vertex_shader, fragment_shader, window.surface_extent,
			builder.vertex_input_topology, builder.vertex_input_bindings,
			builder.vertex_input_attributes, window.image_specs.color_format,
			window.image_specs.depth_format);
	} catch (std::runtime_error& error) {
		return {{}, error.what(), "Failed to create graphics pipeline."};
	}

	return {target, "", ""};
}

OptionalWrapper<VulkanTensor> tensor(const VulkanTensorBuilder& builder, VulkanDevice device)
{
	VulkanTensor tensor;

	std::size_t element_count = 1;
	for (auto dimension : builder.shape) {
		element_count *= dimension;
	}
	tensor.shape = builder.shape;
	tensor.element_size = builder.element_size;
	tensor.element_count = element_count;

	try {
		VkBufferUsageFlags primary_usage_flags = builder.usage;
		primary_usage_flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		primary_usage_flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		tensor.primary_buffer = create_buffer(device.allocator,
			primary_usage_flags, tensor.element_size, tensor.element_count);
	} catch (std::runtime_error& error) {
		return {{}, error.what(), "Failed to create primary buffer."};
	}

	try {
		VkBufferUsageFlags staging_usage_flags = 0;
		staging_usage_flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		staging_usage_flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		tensor.staging_buffer = create_buffer(device.allocator,
			staging_usage_flags, tensor.element_size, tensor.element_count);
	} catch (std::runtime_error& error) {
		return {{}, error.what(), "Failed to create staging buffer."};
	}

	return {tensor, "", ""};
}

}

namespace kodanuki
{

void VulkanWindow::recreate(VulkanDevice device)
{
	*this = vkinit::finalize_dynamic_window(*this, device)
		.expect("Failed to recreate window!");
}

}
