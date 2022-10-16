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

VkDevice create_logical_device(VkPhysicalDevice physical_device, VkDeviceQueueCreateInfo queue_family, std::vector<const char*> extensions)
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
    device_info.enabledExtensionCount = extensions.size();
    device_info.ppEnabledExtensionNames = extensions.data();
    device_info.pEnabledFeatures = &device_features;

    VkDevice result;
    CHECK_VULKAN(vkCreateDevice(physical_device, &device_info, nullptr, &result));
    return result;
}

std::vector<VkQueue> get_queue_handles(VkDevice logical_device, VkDeviceQueueCreateInfo queue_family)
{
    std::vector<VkQueue> result(queue_family.queueCount);
    
    for (int i = 0; i < (int) result.size(); i++) {
        vkGetDeviceQueue(logical_device, queue_family.queueFamilyIndex, 0, &result[i]);
    }
    
    return result;
}

void remove_device(Entity* device)
{
    vkDestroyDevice(ECS::get<VkDevice>(*device), nullptr);
    vkDestroyInstance(ECS::get<VkInstance>(*device), nullptr);
    ECS::remove<Entity>(*device);
    delete device;
}

VulkanDevice::VulkanDevice(DeviceCreateInfo builder)
{
    pimpl = std::shared_ptr<Entity>(new Entity, &remove_device);
    Entity device = *pimpl = ECS::create();
    
    VkInstance instance = create_instance(builder.instance_layers, builder.instance_extensions);
    VkPhysicalDevice physical_device = pick_physical_device(instance, builder.gpu_score);
    VkDeviceQueueCreateInfo queue_family = pick_queue_family(physical_device, builder.queue_score, builder.queue_priorities);
    VkDevice logical_device = create_logical_device(physical_device, queue_family, builder.device_extensions);
    std::vector<VkQueue> queues = get_queue_handles(logical_device, queue_family);

    ECS::update<VkInstance>(device, instance);
    ECS::update<VkPhysicalDevice>(device, physical_device);
    ECS::update<VkDevice>(device, logical_device);
    ECS::update<std::vector<VkQueue>>(device, queues);
}

VkInstance VulkanDevice::get_instance()
{
    return ECS::get<VkInstance>(*pimpl);
}

VkPhysicalDevice VulkanDevice::get_physical_device()
{
    return ECS::get<VkPhysicalDevice>(*pimpl);
}

VkDevice VulkanDevice::get_logical_device()
{
    return ECS::get<VkDevice>(*pimpl);
}

}
