//*/ls > /dev/null && FLAGS="-O2 -std=c++20 -Wall -Wextra -Werror -Wpedantic"
//*/ls > /dev/null && DLIBS="-lvulkan -lsfml-window -lsfml-system"
//usr/bin/cat $0 | g++ $FLAGS $DLIBS -o torus -x c++ - && ./torus
//usr/bin/rm -f torus && exit

/*******************************************************************************
The code below renders a rotating torus with some basic point lights. You
can move through the world and look at it from different angles. On linux
you can execute this file directly, or compile it normally (see above).
It requires the path to matching vertex and fragment shaders.

It is an example on how verbose the vulkan API is. However, it does allow
you to control the GPU rendering process well. It is also very structured
and extensible. But even for simple applications it requires way too much
code. This inherently makes writing wrappers difficult because you probably
loose many features. It is completely independent from the engine and can
be compiled as a standalone (just for testing stuff).
*******************************************************************************/

/*******************************************************************************
MIT License

Copyright (c) 2023 Lukas Niegsch

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*******************************************************************************/
#include <vulkan/vulkan.h>
#include <SFML/Window.hpp>
#include <bits/stdc++.h>

/**
 * Enables debug printing inside the vkutil::autowrapper method.
 */
static constexpr bool ENABLE_AUTOWRAPPER_PRINTING = false; 

/**
 * Makro that terminates the program and prints some debug information.
 */
#define ERROR(reason)							\
	do {										\
		std::stringstream err;					\
		err << (reason) << '\n';				\
		err << "File: " << __FILE__ << '\n';	\
		err << "Line: " << __LINE__ << '\n';	\
		std::cout << err.str();					\
		std::terminate(); 						\
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
 * Reads the complete file into the buffer.
 *
 * @param path The absolute or relative path of the file.
 * @return The buffer containing the contents of the file.
 */
std::vector<char> read_file_into_buffer(std::string path)
{
	std::ifstream file(path, std::ios::binary | std::ios::ate);

	if (!file.is_open()) {
		throw std::invalid_argument(path + " was not found!");
	}

	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<char> buffer(size);
	if (!file.read(buffer.data(), size))
	{
		throw std::runtime_error(path + " could not be read!");
	}

	return buffer;
}

/**
 * Prints the name of the template type.
 * https://stackoverflow.com/questions/81870
 */
template <typename T>
constexpr auto type_name() {
	std::string_view name, prefix, suffix;
#ifdef __clang__
	name = __PRETTY_FUNCTION__;
	prefix = "auto type_name() [T = ";
	suffix = "]";
#elif defined(__GNUC__)
	name = __PRETTY_FUNCTION__;
	prefix = "constexpr auto type_name() [with T = ";
	suffix = "]";
#elif defined(_MSC_VER)
	name = __FUNCSIG__;
	prefix = "auto __cdecl type_name<";
	suffix = ">(void)";
#endif
	name.remove_prefix(prefix.size());
	name.remove_suffix(suffix.size());
	return name;
}

/**
 * Shared wrapper around some given type.
 *
 * Each wrapper basically behaves the same way as a shared_ptr but it
 * also convertes to the base type. This just makes it easier to use
 * them in methods without having to dereference it. Also allows the
 * user to specify some custom destroy method.
 */
template <typename T>
struct shared_wrapper_t
{
public:
	/**
	 * Default constructor so this class plays nicely with std::shared_ptr.
	 */
	shared_wrapper_t() = default;

	/**
	 * Creates a new wrapper around the pointer.
	 *
	 * This wrapper now owns the pointer. It will destroy the pointer once
	 * this class is no longer used.
	 *
	 * @param ptr The pointer which will be wrapped.
	 * @param destroy The custom destroy method.
	 */
	shared_wrapper_t(T* ptr, std::function<void(T*)> destroy)
	{
		this->handle = std::shared_ptr<T>(ptr, destroy);
	}

	/**
	 * Dereferences and returns the underlying type.
	 */
	operator T() const
	{
		if (handle) {
			return *handle;
		}
		return {};
	}

private:
	std::shared_ptr<T> handle;
};

/**
 * Base case for signature matching of function parameters.
 */
template <typename Function, int position, bool reverse>
struct signature {};

/**
 * Matches function pointers and yields the type at the given position.
 */
template <typename R, typename ... Args, int position, bool reverse>
struct signature<R(*)(Args...), position, reverse>
{
	using type = std::conditional_t<reverse,
		std::tuple_element_t<sizeof...(Args) - position - 1, std::tuple<Args...>>,
		std::tuple_element_t<position, std::tuple<Args...>>
	>;
};

/**
 * Base case for signature matching of function parameters.
 */
template <typename Function>
struct function_traits {};

/**
 * Matches function pointers and yields the return and parameter types.
 */
template <typename R, typename ... Args>
struct function_traits<R(*)(Args...)>
{
	using return_type = R;
	using params_type = std::tuple<Args...>;
};

/**
 * Retrieves the argument type at I from some function type (forward).
 */
template <int I, auto Func>
using forward_signature_t = signature<decltype(Func), I, false>::type;

/**
 * Retrieves the argument type at I from some function type (reverse).
 */
template <int I, auto Func>
using reverse_signature_t = signature<decltype(Func), I, true>::type;

/**
 * Retrieves the return type of some function type.
 */
template <auto Func>
using return_signature_t = function_traits<decltype(Func)>::return_type;

/**
 * Retrieves the param types of some function type.
 */
template <auto Func>
using params_signature_t = function_traits<decltype(Func)>::params_type;


namespace vktype
{

using instance_t        = shared_wrapper_t<VkInstance>;
using device_t          = shared_wrapper_t<VkDevice>;
using shader_t          = shared_wrapper_t<VkShaderModule>;
using fence_t           = shared_wrapper_t<VkFence>;
using semaphore_t       = shared_wrapper_t<VkSemaphore>;
using command_pool_t    = shared_wrapper_t<VkCommandPool>;
using command_buffer_t  = shared_wrapper_t<VkCommandBuffer>;
using renderpass_t      = shared_wrapper_t<VkRenderPass>;
using pipeline_t        = shared_wrapper_t<VkPipeline>;
using pipeline_layout_t = shared_wrapper_t<VkPipelineLayout>;
using surface_t         = shared_wrapper_t<VkSurfaceKHR>;
using swapchain_t       = shared_wrapper_t<VkSwapchainKHR>;
using framebuffer_t     = shared_wrapper_t<VkFramebuffer>;
using buffer_t          = shared_wrapper_t<VkBuffer>;
using image_t           = shared_wrapper_t<VkImage>;
using image_view_t      = shared_wrapper_t<VkImageView>;
using memory_t          = shared_wrapper_t<VkDeviceMemory>;
using descriptor_pool_t = shared_wrapper_t<VkDescriptorPool>;

/**
 * The underlying vulkan type for the wrapper.
 *
 * Example:
 * vkCreateDevice    -> VkDevice
 * vkCreateSemaphore -> VkSemaphore
 */
template <auto fn_create>
using vulkan_t = std::remove_pointer_t<reverse_signature_t<0, fn_create>>;

/**
 * The create info type for the create method.
 *
 * Example:
 * vkCreateDevice    -> VkDeviceCreateInfo
 * vkCreateSemaphore -> VkSemaphoreCreateInfo
 */
template <auto fn_create, bool allocate = false>
using config_t = std::remove_const_t<std::remove_pointer_t<reverse_signature_t<2 - allocate, fn_create>>>;

/**
 * Contains information about physical devices and queue families.
 */
struct gpu_specs_t
{
	VkPhysicalDevice physical_device;
	VkPhysicalDeviceProperties physical_device_properties;
	VkPhysicalDeviceFeatures physical_device_features;
	VkPhysicalDeviceMemoryProperties physical_device_memory_properties;
	VkQueueFamilyProperties queue_family_properties;
	uint32_t queue_family_index;
};

/**
 * Contains information needed for rendering the swapchain images.
 */
struct img_specs_t
{
	VkFormat depth_format;
	VkFormat color_format;
	VkColorSpaceKHR color_space;
	VkPresentModeKHR present_mode;
	uint32_t frame_count;
};

/**
 * Contains the per frame image information needed for rendering.
 */
struct frame_t
{
	vktype::image_view_t render_image;
	vktype::framebuffer_t framebuffer;
	vktype::command_buffer_t command_buffer;
	vktype::semaphore_t image_available_semaphore;
	vktype::semaphore_t render_finished_semaphore;
	vktype::fence_t aquire_frame_fence;
};

/**
 * Contains all the information required to render to the target.
 */
struct target_t
{
	vktype::surface_t surface;
	vktype::swapchain_t swapchain;
	vktype::img_specs_t img_specs;
	vktype::command_pool_t command_pool;
	vktype::image_t depth_image;
	vktype::image_view_t depth_image_view;
	vktype::memory_t depth_image_memory;
	std::vector<frame_t> frames;
	uint32_t submit_frame;
	uint32_t render_frame;
	uint32_t max_frame;
};

}


namespace vkutil
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
 *
 * @param args The optional arguments for the passed function.
 */
template <auto fn_enumerate, typename ... Args>
auto vectorize(Args ... args)
{
	using T = vktype::vulkan_t<fn_enumerate>;
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
auto autowrapper(
    const vktype::config_t<fn_create, !std::is_same_v<const VkAllocationCallbacks*,
    reverse_signature_t<1, fn_create>>>& config, args_t ... args)
{
    using T = vktype::vulkan_t<fn_create>;
    constexpr bool is_allocate_wrapper = sizeof...(args_t) == 2;

    auto destroy = [=](T* ptr) {
		if constexpr (ENABLE_AUTOWRAPPER_PRINTING) {
        	std::cout << "[INFO] delete: " << type_name<T>() << std::endl;
		}
        if constexpr (std::is_same_v<T, VkPipeline>) {
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

	if constexpr (ENABLE_AUTOWRAPPER_PRINTING) {
		std::cout << "[INFO] create: " << type_name<T>() << std::endl;
	}
	T* output = new T();
    if constexpr (std::is_same_v<T, VkPipeline>) {
        CHECK_VULKAN(fn_create(args..., VK_NULL_HANDLE, 1, &config, nullptr, output));
    } else if constexpr (is_allocate_wrapper) {
        auto[device, _] = std::tie(args...);
        CHECK_VULKAN(fn_create(device, &config, output));
    } else {
        CHECK_VULKAN(fn_create(args..., &config, nullptr, output));
    }
	return shared_wrapper_t<T>(output, destroy);
}

}


namespace vkinit
{

/**
 * Vulkan instances hold the context of the application.
 * 
 * @param layers The layers that should be enabled.
 * @param extensions The extensions that should be used.
 */
vktype::instance_t instance(
	std::vector<const char*> layers,
	std::vector<const char*> extensions)
{
	VkApplicationInfo info = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pNext = nullptr,
		.pApplicationName = nullptr,
		.applicationVersion = VK_MAKE_VERSION(0, 0, 0),
		.pEngineName = "kodanuki",
		.engineVersion = VK_MAKE_VERSION(0, 0, 1),
		.apiVersion = VK_API_VERSION_1_3
	};
	return vkutil::autowrapper<vkCreateInstance, vkDestroyInstance>({
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.pApplicationInfo = &info,
		.enabledLayerCount = static_cast<uint32_t>(layers.size()),
		.ppEnabledLayerNames = layers.data(),
		.enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
		.ppEnabledExtensionNames = extensions.data()
	});
}

/**
 * Returns the vector of all different gpu specs.
 * 
 * There will be one for each physical devices and queue family
 * pair. This way there might be multiple entries for the same
 * physical device. The specs contains the properties and features
 * of the physical device and the queue family.
 *
 * @param instance The instance for which to enumerate the devices.
 */
std::vector<vktype::gpu_specs_t> query_physical_devices(VkInstance instance)
{
	std::vector<vktype::gpu_specs_t> devices;
	
	for (auto device : vkutil::vectorize<vkEnumeratePhysicalDevices>(instance))
	{
		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(device, &properties);

		VkPhysicalDeviceFeatures features;
		vkGetPhysicalDeviceFeatures(device, &features);

		VkPhysicalDeviceMemoryProperties memory_properties;
		vkGetPhysicalDeviceMemoryProperties(device, &memory_properties);

		uint32_t queue_family_index = 0;
		for (auto queue : vkutil::vectorize<vkGetPhysicalDeviceQueueFamilyProperties>(device))
		{	
			vktype::gpu_specs_t specs = {
				device, properties, features, memory_properties,
				queue, queue_family_index};

			devices.push_back(specs);
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
vktype::gpu_specs_t select_physical_device(VkInstance instance, std::function<int(vktype::gpu_specs_t)> fn_score)
{
	std::vector<vktype::gpu_specs_t> devices = query_physical_devices(instance);
	return *std::ranges::max_element(devices, std::ranges::less{}, fn_score);
}

/**
 * Vulkan devices are the logical representation of the GPU.
 *
 * @param specs The specs of the physical device.
 * @param extensions The extensions that the device should use.
 * @param queue_priorities One priority for each created queue.
 */
vktype::device_t device(
	vktype::gpu_specs_t      specs,
	std::vector<const char*> extensions,
	std::vector<float>       queue_priorities)
{
	VkDeviceQueueCreateInfo info = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.queueFamilyIndex = specs.queue_family_index,
		.queueCount = static_cast<uint32_t>(queue_priorities.size()),
		.pQueuePriorities = queue_priorities.data(),
	};
	return vkutil::autowrapper<vkCreateDevice, vkDestroyDevice>({
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &info,
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = nullptr,
		.enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
		.ppEnabledExtensionNames = extensions.data(),
		.pEnabledFeatures = &specs.physical_device_features,
	}, specs.physical_device);
}

/**
 * Vulkan swapchains replace the images for the some surface.
 *
 * @param device The device that stores the swapchain.
 * @param surface The surface on which to place the images.
 * @param specs The configuration for the images.
 */
vktype::swapchain_t swapchain(
	vktype::device_t    device,
	vktype::surface_t   surface,
	vktype::img_specs_t specs,
	VkExtent2D          surface_extent)
{
	return vkutil::autowrapper<vkCreateSwapchainKHR, vkDestroySwapchainKHR>({
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
		.oldSwapchain = VK_NULL_HANDLE,
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
vktype::surface_t surface(
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
vktype::image_view_t image_view(
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
	return vkutil::autowrapper<vkCreateImageView, vkDestroyImageView>({
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
vktype::fence_t fence(
	vktype::device_t      device,
	VkFenceCreateFlagBits flags)
{
	return vkutil::autowrapper<vkCreateFence, vkDestroyFence>({
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
vktype::semaphore_t semaphore(
	vktype::device_t device)
{
	return vkutil::autowrapper<vkCreateSemaphore, vkDestroySemaphore>({
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0
	}, device);
}

/**
 * Vulkan command pools allocate command buffers.
 *
 * @param device The device that stores the command pool.
 * @param queue_family_index The index of the queue family.
 */
vktype::command_pool_t command_pool(
	vktype::device_t device,
	uint32_t queue_family_index)
{
	return vkutil::autowrapper<vkCreateCommandPool, vkDestroyCommandPool>({
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
vktype::command_buffer_t command_buffer(
	vktype::device_t       device,
	vktype::command_pool_t pool)
{
	return vkutil::autowrapper<vkAllocateCommandBuffers, vkFreeCommandBuffers>({
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
vktype::descriptor_pool_t descriptor_pool(
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
	return vkutil::autowrapper<vkCreateDescriptorPool, vkDestroyDescriptorPool>({
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.pNext = nullptr,
		.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
		.maxSets = max_uniform_set_size * pool_sizes_count,
		.poolSizeCount = pool_sizes_count,
		.pPoolSizes = pool_sizes.data()
	}, device);
}

/**
 * Vulkan shader modules hold the SPIR-V shader code for compilation.
 *
 * @param device The device that stores the shader.
 * @param file_path The path to the SPIR-V file, e.g. example.frag.
 */
vktype::shader_t shader(
	vktype::device_t device,
	const char*      file_path)
{
	std::vector<char> code = read_file_into_buffer(file_path);
	return vkutil::autowrapper<vkCreateShaderModule, vkDestroyShaderModule>({
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
 * @param device The device that stores the image.
 * @param image_format The format of the image.
 * @param image_extent The suface extent in 2D of the image.
 * @param image_usage The usage bits for the image.
 */
vktype::image_t image(
	vktype::device_t  device,
	VkFormat          image_format,
	VkExtent2D        image_extent,
	VkImageUsageFlags image_usage)
{
	return vkutil::autowrapper<vkCreateImage, vkDestroyImage>({
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
	}, device);
}

/**
 * Vulkan device memory is actual memory on the GPU (device) or CPU (host).
 *
 * @param device The device that stores handles (and maybe actual memory).
 * @param specs The configuration of the physical device.
 * @param requirements The requirements for the memory (size, alignment).
 * @param properties The properties such as where the memory is allocated.
 */
vktype::memory_t memory(
	vktype::device_t      device,
	vktype::gpu_specs_t   specs,
	VkMemoryRequirements  requirements,
	VkMemoryPropertyFlags properties)
{
	std::optional<uint32_t> type_index;
	for (uint32_t i = 0; i < specs.physical_device_memory_properties.memoryTypeCount; i++) {
		if (!(requirements.memoryTypeBits & (1 << i))) {
			continue;
		}
		if ((specs.physical_device_memory_properties.memoryTypes[i].propertyFlags & properties) != properties) {
			continue;
		}
		type_index = i;
		break;
	}
	if (!type_index) {
		throw std::invalid_argument("Failed to find suitable memory type!");
	}
	return vkutil::autowrapper<vkAllocateMemory, vkFreeMemory>({
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.pNext = nullptr,
		.allocationSize = requirements.size,
		.memoryTypeIndex = type_index.value()
	}, device);
}

/**
 * Vulkan renderpasses contain subpasses and their dependencies / attachments.
 * For each subpass one graphics pipeline is executed. The renderpass defines
 * how inputs / outputs between these pipelines depend on each other.
 *
 * Here we define a very basic renderpass consisting of one subpass. It only
 * has one color attachment and depth attachment.
 *
 * @param device The device that stores the renderpass.
 * @param specs The image formats for the color and depth images.
 */
vktype::renderpass_t renderpass(
	vktype::device_t    device,
	vktype::img_specs_t specs)
{
	VkAttachmentDescription color_attachment = {
		.flags = 0,
		.format = specs.color_format,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
	};
	VkAttachmentDescription depth_attachment = {
		.flags = 0,
		.format = specs.depth_format,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
	};
	std::array<VkAttachmentDescription, 2> attachments = {
		color_attachment,
		depth_attachment
	};
	VkAttachmentReference color_reference = {
		.attachment = 0,
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	};
	VkAttachmentReference depth_reference = {
		.attachment = 1,
		.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
	};
	VkSubpassDescription subpass = {
		.flags = 0,
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.inputAttachmentCount = 0,
		.pInputAttachments = nullptr,
		.colorAttachmentCount = 1,
		.pColorAttachments = &color_reference,
		.pResolveAttachments = nullptr,
		.pDepthStencilAttachment = &depth_reference,
		.preserveAttachmentCount = 0,
		.pPreserveAttachments = nullptr,
	};
	return vkutil::autowrapper<vkCreateRenderPass, vkDestroyRenderPass>({
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.attachmentCount = attachments.size(),
		.pAttachments = attachments.data(),
		.subpassCount = 1,
		.pSubpasses = &subpass,
		.dependencyCount = 0,
		.pDependencies = nullptr,
	}, device);
}

/**
 * Vulkan framebuffers stores a collection of attachments used by the
 * renderpass. This is the actual memory bound during the pipelines.
 * Images stored here use surface corrdinates.
 *
 * @param device The device that stores the framebuffer.
 * @param renderpass The renderpass that uses the attachments.
 * @param surface_extent The current extent of the surface.
 * @param attachments The image views for the attachment images.
 */
vktype::framebuffer_t framebuffer(
	vktype::device_t     device,
	vktype::renderpass_t renderpass,
	VkExtent2D           surface_extent,
	std::vector<VkImageView> attachments)
{
	return vkutil::autowrapper<vkCreateFramebuffer, vkDestroyFramebuffer>({
		.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.renderPass = renderpass,
		.attachmentCount = static_cast<uint32_t>(attachments.size()),
		.pAttachments = attachments.data(),
		.width = surface_extent.width,
		.height = surface_extent.height,
		.layers = 1
	}, device);
}

/**
 * The target is a custom type that stores the swapchain and the
 * frames and information needed to display these.
 *
 * The target has to be recreated when the surface changes, e.g. when
 * the window was resized. Target recreation works by calling this
 * method again and overwriting the previous variable.
 *
 * @param device The device which renders the image.
 * @param renderpass The renderpass that renders images.
 * @param gpu_specs The configuration of the device.
 * @param img_specs The configuration for the image.
 * @param surface The surface on which to render.
 * @param surface_extent The current extent of the surface.
 */
vktype::target_t target(
	vktype::device_t     device,
	vktype::renderpass_t renderpass,
	vktype::gpu_specs_t  gpu_specs,
	vktype::img_specs_t  img_specs,
	vktype::surface_t    surface,
	VkExtent2D           surface_extent)
{
	vktype::swapchain_t swapchain = vkinit::swapchain(
		device, surface, img_specs, surface_extent);

	vktype::command_pool_t command_pool = vkinit::command_pool(
		device, gpu_specs.queue_family_index);

	vktype::image_t depth_image = vkinit::image(
		device, img_specs.depth_format, surface_extent,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);

	VkMemoryRequirements memory_requirements;
	vkGetImageMemoryRequirements(device, depth_image, &memory_requirements);
	vktype::memory_t depth_image_memory = vkinit::memory(
		device, gpu_specs, memory_requirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	vkBindImageMemory(device, depth_image, depth_image_memory, 0);

	vktype::image_view_t depth_image_view = vkinit::image_view(
		device, img_specs.depth_format, depth_image, VK_IMAGE_ASPECT_DEPTH_BIT);

	vktype::target_t result;
	result.surface = surface;
	result.img_specs = img_specs; 
	result.swapchain = swapchain;
	result.command_pool = command_pool;
	result.depth_image = depth_image;
	result.depth_image_view = depth_image_view;
	result.depth_image_memory = depth_image_memory;
	result.submit_frame = 0;
	result.render_frame = 0;
	result.max_frame = img_specs.frame_count;

	for (VkImage image : vkutil::vectorize<vkGetSwapchainImagesKHR>(device, swapchain)) {
		vktype::frame_t frame;
		frame.render_image = vkinit::image_view(device, img_specs.color_format, image, VK_IMAGE_ASPECT_COLOR_BIT);
		frame.image_available_semaphore = vkinit::semaphore(device);
		frame.render_finished_semaphore = vkinit::semaphore(device);
		frame.aquire_frame_fence = vkinit::fence(device, VK_FENCE_CREATE_SIGNALED_BIT);
		frame.command_buffer = vkinit::command_buffer(device, command_pool);
		frame.framebuffer = vkinit::framebuffer(device, renderpass, surface_extent, {frame.render_image, depth_image_view});
		result.frames.push_back(frame);
	}

	return result;
}

}


namespace vkdraw
{

/**
 * The timeout for drawing commands in (almost) nanoseconds.
 */
static uint64_t TIMEOUT = 1 /* seconds */ * 100000000;

/**
 * Aquires the current frame for rendering.
 *
 * @param device The device that renders the frame.
 * @param target The target on which to render.
 * @return Were there no swapchain errors?
 */
bool aquire_frame(
	vktype::device_t  device,
	vktype::target_t& target)
{
	vktype::frame_t& frame = target.frames[target.submit_frame];
	VkFence aquire_fence = frame.aquire_frame_fence;
	CHECK_VULKAN(vkWaitForFences(device, 1, &aquire_fence, VK_TRUE, TIMEOUT));

	auto result = vkAcquireNextImageKHR(device, target.swapchain, TIMEOUT,
		frame.image_available_semaphore, VK_NULL_HANDLE, &target.render_frame);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		return false;
	}

	if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	CHECK_VULKAN(vkResetFences(device, 1, &aquire_fence));
	return true;
}

/**
 * Submits the draw commands to the GPU. This will start one renderpass
 * and binds one pipeline, and then calls all the command before submitting.
 *
 * @param target The target on which to render.
 * @param renderpass The renderpass for which to render.
 * @param pipeline The pipeline for the single subpass.
 * @param surface_extent The render area (typically the window extent).
 * @param submit_queue The queue on which to submit the commands.
 * @param commands The actual draw commands.
 */
void submit_frame(
	vktype::target_t&                                 target,
	vktype::renderpass_t                              renderpass,
	vktype::pipeline_t                                pipeline,
	VkExtent2D                                        surface_extent,
	VkQueue                                           submit_queue,
	std::vector<std::function<void(VkCommandBuffer)>> commands)
{
	vktype::frame_t& frame = target.frames[target.submit_frame];
	VkCommandBuffer buffer = frame.command_buffer;

	VkCommandBufferBeginInfo buffer_info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = nullptr,
		.flags = 0,
		.pInheritanceInfo = nullptr
	};
	CHECK_VULKAN(vkResetCommandBuffer(buffer, 0));
	CHECK_VULKAN(vkBeginCommandBuffer(buffer, &buffer_info));

	std::array<VkClearValue, 2> clear_values = {};
	clear_values[0].color = {0.53f, 0.81f, 0.92f};
	clear_values[1].depthStencil = {1.0f, 0};

	VkRenderPassBeginInfo renderpass_info = {};
	renderpass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderpass_info.renderPass = renderpass;
	renderpass_info.framebuffer = frame.framebuffer;
	renderpass_info.renderArea.offset = {0, 0};
	renderpass_info.renderArea.extent = surface_extent;
	renderpass_info.clearValueCount = clear_values.size();
	renderpass_info.pClearValues = clear_values.data();
	vkCmdBeginRenderPass(buffer, &renderpass_info, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

	for (auto command_callback : commands) {
		command_callback(buffer);
	}
	
	vkCmdEndRenderPass(buffer);
	CHECK_VULKAN(vkEndCommandBuffer(buffer));

	VkSemaphore image_available = frame.image_available_semaphore;
	VkSemaphore render_finished = frame.render_finished_semaphore;
	VkFence aquire_frame = frame.aquire_frame_fence;
	VkPipelineStageFlags flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	VkSubmitInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	info.waitSemaphoreCount = 1;
	info.pWaitSemaphores = &image_available;
	info.pWaitDstStageMask = &flags;
	info.commandBufferCount = 1;
	info.pCommandBuffers = &buffer;
	info.signalSemaphoreCount = 1;
	info.pSignalSemaphores = &render_finished;
	CHECK_VULKAN(vkQueueSubmit(submit_queue, 1, &info, aquire_frame));
	target.submit_frame = (target.submit_frame + 1) % target.max_frame;
}

/**
 * Presents the current frame for rendering.
 *
 * @param target The target on which to render.
 * @param submit_queue The queue on which to render.
 * @return Were there no swapchain errors?
 */
bool render_frame(
	vktype::target_t& target,
	VkQueue           render_queue)
{
	vktype::frame_t& frame = target.frames[target.render_frame];
	VkSwapchainKHR swapchain = target.swapchain;
	VkSemaphore render_finished = frame.render_finished_semaphore;
	
	VkPresentInfoKHR info = {};
	info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	info.waitSemaphoreCount = 1;
	info.pWaitSemaphores = &render_finished;
	info.swapchainCount = 1;
	info.pSwapchains = &swapchain;
	info.pImageIndices = &target.render_frame;
	auto result = vkQueuePresentKHR(render_queue, &info);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		return false;
	}

	if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}

	return true;
}

}

/**
 * Most people don't have multiple discrete GPU's so we provide a simple
 * method that picks one of them. The devices must have at least one queue
 * with the graphics bit enabled.
 *
 * @param specs The physical device properties to score.
 */
int score_device(vktype::gpu_specs_t specs)
{
	int score = 0;
	score += specs.physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
	score *= specs.queue_family_properties.queueCount;
	score *= specs.queue_family_properties.queueFlags & VK_QUEUE_GRAPHICS_BIT;
	return score;
}

/**
 * Returns the current window extent.
 *
 * @param window The reference to the SFML window.
 */
VkExtent2D get_window_extent(sf::WindowBase& window)
{
	sf::Vector2u size = window.getSize();
	VkExtent2D extent;
	extent.width = static_cast<uint32_t>(size.x);
	extent.height = static_cast<uint32_t>(size.y);
	return extent;
}

int main()
{
	sf::WindowBase window(sf::VideoMode(1950, 1200), "Torus");
	VkExtent2D extent = get_window_extent(window);

	vktype::instance_t instance = vkinit::instance(
		{"VK_LAYER_KHRONOS_validation"},
		sf::Vulkan::getGraphicsRequiredInstanceExtensions());

	vktype::gpu_specs_t gpu_specs = vkinit::select_physical_device(
		instance, &score_device);

	vktype::device_t device = vkinit::device(
		gpu_specs, {"VK_KHR_swapchain"}, {1.0f});

	vktype::surface_t surface = vkinit::surface(
		instance, [&](VkInstance instance, VkSurfaceKHR& surface) {
			window.createVulkanSurface(instance, surface); });

	vktype::img_specs_t img_specs = {
		.depth_format = VK_FORMAT_D32_SFLOAT_S8_UINT,
		.color_format = VK_FORMAT_B8G8R8A8_UNORM,
		.color_space = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
		.present_mode = VK_PRESENT_MODE_IMMEDIATE_KHR,
		.frame_count = 4
	};

	vktype::renderpass_t renderpass = vkinit::renderpass(
		device, img_specs);

	vktype::target_t target = vkinit::target(
		device, renderpass, gpu_specs, img_specs, surface, extent);


	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}

		// if (!vkdraw::aquire_frame(device, target)) {
		// 	target = vkinit::target(device, renderpass, gpu_specs,
		//		img_specs, surface, get_window_extent(window));
		// 	continue;
		// }
	}

	return 0;
}
