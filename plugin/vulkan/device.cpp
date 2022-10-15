#include "plugin/vulkan/device.h"
#include "plugin/vulkan/debug.h"
#include <cassert>

namespace Kodanuki
{

VkInstance create_instance(std::vector<const char*> layers, std::vector<const char*> extensions)
{
    VkApplicationInfo app_info;
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pNext = nullptr;
    app_info.pApplicationName = nullptr;
    app_info.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
    app_info.pEngineName = "Kodanuki";
    app_info.engineVersion = VK_MAKE_VERSION(0, 0, 1);
    app_info.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo instance_info;
    instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_info.pNext = nullptr;
    instance_info.flags = 0;
    instance_info.pApplicationInfo = &app_info;
    instance_info.enabledLayerCount = layers.size();
    instance_info.ppEnabledLayerNames = layers.data();
    instance_info.enabledExtensionCount = extensions.size();
    instance_info.ppEnabledExtensionNames = extensions.data();

    VkInstance result;
    CHECK_VULKAN(vkCreateInstance(&instance_info, nullptr, &result));
    return result;
}

VkPhysicalDevice pick_physical_device(VkInstance instance, std::function<int(VkPhysicalDevice)> gpu_score)
{
    auto devices = vectorize<vkEnumeratePhysicalDevices>(instance);
    auto compare = [&](auto a, auto b) { return gpu_score(a) < gpu_score(b); };
    assert(devices.size() > 0); // No graphics card available!
    return *std::max_element(devices.begin(), devices.end(), compare);
}

VkDeviceQueueCreateInfo pick_queue_family(VkPhysicalDevice device, std::function<int(VkQueueFamilyProperties)> queue_score, std::vector<float>& properties)
{
    auto queues = vectorize<vkGetPhysicalDeviceQueueFamilyProperties>(device);
    auto compare = [&](auto a, auto b) { return queue_score(a) < queue_score(b); };
    auto index = std::distance(queues.begin(), std::max_element(queues.begin(), queues.end(), compare));

    VkDeviceQueueCreateInfo queue_info;
    queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_info.pNext = nullptr;
    queue_info.flags = 0;
    queue_info.queueFamilyIndex = index;
    queue_info.queueCount = properties.size();
    queue_info.pQueuePriorities = properties.data();

    return queue_info;
}

VkDevice create_logical_device(VkPhysicalDevice physical_device, VkDeviceQueueCreateInfo queue_family)
{
    VkPhysicalDeviceFeatures device_features = {};

    VkDeviceCreateInfo device_info;
    device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_info.pNext = nullptr;
    device_info.flags = 0;
    device_info.queueCreateInfoCount = 1;
    device_info.pQueueCreateInfos = &queue_family;
    device_info.enabledLayerCount = 0;
    device_info.ppEnabledLayerNames = nullptr;
    device_info.enabledExtensionCount = 0;
    device_info.ppEnabledExtensionNames = nullptr;
    device_info.pEnabledFeatures = &device_features;

    VkDevice result;
    CHECK_VULKAN(vkCreateDevice(physical_device, &device_info, nullptr, &result));
    return result;
}

Device create_device(DeviceCreateInfo seed)
{
    Entity device = ECS::create();

    VkInstance instance = create_instance(seed.enabled_layers, seed.enabled_extensions);
    VkPhysicalDevice physical_device = pick_physical_device(instance, seed.gpu_score);
    VkDeviceQueueCreateInfo queue_family = pick_queue_family(physical_device, seed.queue_score, seed.queue_priorities);
    VkDevice logical_device = create_logical_device(physical_device, queue_family);

    ECS::update<VkInstance>(device, instance);
    ECS::update<VkPhysicalDevice>(device, physical_device);
    ECS::update<VkDevice>(device, logical_device);
    
    return device;
}

void remove_device(Device device)
{
    vkDestroyDevice(ECS::get<VkDevice>(device), nullptr);
    vkDestroyInstance(ECS::get<VkInstance>(device), nullptr);
    ECS::remove<Entity>(device);
}

}
