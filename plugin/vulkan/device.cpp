#include "plugin/vulkan/device.h"
#include "plugin/vulkan/debug.h"

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

Device create_device(DeviceCreateInfo seed)
{
    Entity device = ECS::create();

    VkInstance instance = create_instance(seed.enabled_layers, seed.enabled_extensions);

    ECS::update<VkInstance>(device, instance);
    return device;
}

void remove_device(Device device)
{
    VkInstance instance = ECS::get<VkInstance>(device);

    vkDestroyInstance(instance, nullptr);
}

}
