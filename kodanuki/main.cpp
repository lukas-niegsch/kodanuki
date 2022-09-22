#include "engine/central/entity.h"
#include "engine/central/archetype.h"
#include "engine/concept/family.h"
#include "plugin/vulkan/debug.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
using namespace Kodanuki;

// Global variables
VkInstance g_instance;

void create_vulkan_instance()
{
	VkApplicationInfo app_info = {};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = "Kodanuki";
	app_info.pEngineName = "Kodanuki";
	app_info.apiVersion = VK_API_VERSION_1_3;
	
	print_vulkan_info(vectorize<vkEnumerateInstanceExtensionProperties>(nullptr));
	print_vulkan_info(vectorize<vkEnumerateInstanceLayerProperties>());

	uint32_t count;
	auto glfw_extensions = glfwGetRequiredInstanceExtensions(&count);
	std::vector<const char*> extensions(glfw_extensions, glfw_extensions + count);
	std::vector<const char*> layers = {"VK_LAYER_KHRONOS_validation"};

	VkInstanceCreateInfo instance_info = {};
	instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instance_info.pApplicationInfo = &app_info;
	instance_info.enabledLayerCount = layers.size();
	instance_info.ppEnabledLayerNames = layers.data();
	instance_info.enabledExtensionCount = extensions.size();
	instance_info.ppEnabledExtensionNames = extensions.data();

	vkCreateInstance(&instance_info, nullptr, &g_instance);
}

int main()
{
	create_vulkan_instance();
	print_vulkan_info(vectorize<vkEnumeratePhysicalDevices>(g_instance));
	vkDestroyInstance(g_instance, nullptr);
	return 0;
}

