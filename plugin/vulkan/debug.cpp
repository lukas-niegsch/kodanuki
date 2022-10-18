#include "plugin/vulkan/debug.h"

namespace Kodanuki
{

void print_vulkan_version(std::string name, uint32_t version)
{
	std::cout << name << ": ";
	std::cout << VK_VERSION_MAJOR(version) << ".";
	std::cout << VK_VERSION_MINOR(version) << ".";
	std::cout << VK_VERSION_PATCH(version) << "\n";
}

void print_vulkan_extent(std::string name, VkExtent2D extent)
{
	std::cout << name << ": " << extent.width << ' ' << extent.height << '\n';
}

//////////////////////////////// Specializations ///////////////////////////////
template <>
void print_vulkan_struct(VkExtensionProperties info)
{
	std::cout << "extensionName: " << info.extensionName << '\n';
	print_vulkan_version("specVersion", info.specVersion);
}

template <>
void print_vulkan_struct(VkLayerProperties info)
{
	std::cout << "layerName: " << info.layerName << '\n';
	print_vulkan_version("specVersion", info.specVersion);
	print_vulkan_version("implementationVersion", info.implementationVersion);
	std::cout << "description: " << info.description << '\n';
}

template <>
void print_vulkan_struct(VkPhysicalDeviceProperties info)
{
	print_vulkan_version("apiVersion", info.apiVersion);
	print_vulkan_version("driverVersion", info.driverVersion);
	std::cout << "vendorID: " << info.vendorID << '\n';
	std::cout << "deviceID: " << info.deviceID << '\n';
	std::cout << "deviceType: ";
	switch(info.deviceType)
	{
	case VK_PHYSICAL_DEVICE_TYPE_OTHER:
		std::cout << "VK_PHYSICAL_DEVICE_TYPE_OTHER";
		break;
	case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
		std::cout << "VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU";
		break;
	case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
		std::cout << "VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU";
		break;
	case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
		std::cout << "VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU";
		break;
	case VK_PHYSICAL_DEVICE_TYPE_CPU:
		std::cout << "VK_PHYSICAL_DEVICE_TYPE_CPU";
		break;
	default:
		std::cout << "unknown";
	}
	std::cout << '\n';
	std::cout << "deviceName: " << info.deviceName << '\n';

	std::cout << "pipelineCacheUUID:";
	for (uint32_t i = 0; i < VK_UUID_SIZE; i++)
	{
		std::cout << " " << +info.pipelineCacheUUID[i];
	}
	std::cout << '\n';
}

template <>
void print_vulkan_struct(VkQueueFamilyProperties info)
{
	#define PRINT_ENUM_FLAG(name) \
		std::cout << #name << ": " << (info.queueFlags & name ? "true" : "false") << '\n'

	std::cout << "queueFlags: " << '\n';
	PRINT_ENUM_FLAG(VK_QUEUE_GRAPHICS_BIT);
	PRINT_ENUM_FLAG(VK_QUEUE_COMPUTE_BIT);
	PRINT_ENUM_FLAG(VK_QUEUE_TRANSFER_BIT);
	PRINT_ENUM_FLAG(VK_QUEUE_SPARSE_BINDING_BIT);
	PRINT_ENUM_FLAG(VK_QUEUE_PROTECTED_BIT);

	#undef PRINT_ENUM_FLAG

	std::cout << "queueCount: " << info.queueCount << '\n';
	std::cout << "timestampValidBits: " << info.timestampValidBits << '\n';
	std::cout << "minImageTransferGranularity: ";
	std::cout << info.minImageTransferGranularity.width << "x";
	std::cout << info.minImageTransferGranularity.height << "x";
	std::cout << info.minImageTransferGranularity.depth << '\n';
}

template <>
void print_vulkan_struct(VkPhysicalDeviceFeatures info)
{
	#define PRINT_VULKAN_BOOL(name) \
		std::cout << #name << ": " << (info.name ? "true" : "false") << '\n'

	PRINT_VULKAN_BOOL(robustBufferAccess);
	PRINT_VULKAN_BOOL(fullDrawIndexUint32);
	PRINT_VULKAN_BOOL(imageCubeArray);
	PRINT_VULKAN_BOOL(independentBlend);
	PRINT_VULKAN_BOOL(geometryShader);
	PRINT_VULKAN_BOOL(tessellationShader);
	PRINT_VULKAN_BOOL(sampleRateShading);
	PRINT_VULKAN_BOOL(dualSrcBlend);
	PRINT_VULKAN_BOOL(logicOp);
	PRINT_VULKAN_BOOL(multiDrawIndirect);
	PRINT_VULKAN_BOOL(drawIndirectFirstInstance);
	PRINT_VULKAN_BOOL(depthClamp);
	PRINT_VULKAN_BOOL(depthBiasClamp);
	PRINT_VULKAN_BOOL(fillModeNonSolid);
	PRINT_VULKAN_BOOL(depthBounds);
	PRINT_VULKAN_BOOL(wideLines);
	PRINT_VULKAN_BOOL(largePoints);
	PRINT_VULKAN_BOOL(alphaToOne);
	PRINT_VULKAN_BOOL(multiViewport);
	PRINT_VULKAN_BOOL(samplerAnisotropy);
	PRINT_VULKAN_BOOL(textureCompressionETC2);
	PRINT_VULKAN_BOOL(textureCompressionASTC_LDR);
	PRINT_VULKAN_BOOL(textureCompressionBC);
	PRINT_VULKAN_BOOL(occlusionQueryPrecise);
	PRINT_VULKAN_BOOL(pipelineStatisticsQuery);
	PRINT_VULKAN_BOOL(vertexPipelineStoresAndAtomics);
	PRINT_VULKAN_BOOL(fragmentStoresAndAtomics);
	PRINT_VULKAN_BOOL(shaderTessellationAndGeometryPointSize);
	PRINT_VULKAN_BOOL(shaderImageGatherExtended);
	PRINT_VULKAN_BOOL(shaderStorageImageExtendedFormats);
	PRINT_VULKAN_BOOL(shaderStorageImageMultisample);
	PRINT_VULKAN_BOOL(shaderStorageImageReadWithoutFormat);
	PRINT_VULKAN_BOOL(shaderStorageImageWriteWithoutFormat);
	PRINT_VULKAN_BOOL(shaderUniformBufferArrayDynamicIndexing);
	PRINT_VULKAN_BOOL(shaderSampledImageArrayDynamicIndexing);
	PRINT_VULKAN_BOOL(shaderStorageBufferArrayDynamicIndexing);
	PRINT_VULKAN_BOOL(shaderStorageImageArrayDynamicIndexing);
	PRINT_VULKAN_BOOL(shaderClipDistance);
	PRINT_VULKAN_BOOL(shaderCullDistance);
	PRINT_VULKAN_BOOL(shaderFloat64);
	PRINT_VULKAN_BOOL(shaderInt64);
	PRINT_VULKAN_BOOL(shaderInt16);
	PRINT_VULKAN_BOOL(shaderResourceResidency);
	PRINT_VULKAN_BOOL(shaderResourceMinLod);
	PRINT_VULKAN_BOOL(sparseBinding);
	PRINT_VULKAN_BOOL(sparseResidencyBuffer);
	PRINT_VULKAN_BOOL(sparseResidencyImage2D);
	PRINT_VULKAN_BOOL(sparseResidencyImage3D);
	PRINT_VULKAN_BOOL(sparseResidency2Samples);
	PRINT_VULKAN_BOOL(sparseResidency4Samples);
	PRINT_VULKAN_BOOL(sparseResidency8Samples);
	PRINT_VULKAN_BOOL(sparseResidency16Samples);
	PRINT_VULKAN_BOOL(sparseResidencyAliased);
	PRINT_VULKAN_BOOL(variableMultisampleRate);
	PRINT_VULKAN_BOOL(inheritedQueries);

	#undef PRINT_VULKAN_BOOL
}

template <>
void print_vulkan_info(VkPhysicalDevice info)
{
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(info, &properties);
	print_vulkan_info(properties);

	auto queue_families = vectorize<vkGetPhysicalDeviceQueueFamilyProperties>(info);
	print_vulkan_info(queue_families);

	VkPhysicalDeviceFeatures features;
	vkGetPhysicalDeviceFeatures(info, &features);
	print_vulkan_info(features);
}

template <>
void print_vulkan_struct(VkSurfaceFormatKHR info)
{
	std::cout << "format: " << info.format << '\n';
	std::cout << "colorSpace: " << info.colorSpace << '\n';
}

template <>
void print_vulkan_struct(VkPresentModeKHR info)
{
	std::cout << "mode: ";
	switch(info)
	{
	case VK_PRESENT_MODE_IMMEDIATE_KHR:
		std::cout << "VK_PRESENT_MODE_IMMEDIATE_KHR";
		break;
	case VK_PRESENT_MODE_MAILBOX_KHR:
		std::cout << "VK_PRESENT_MODE_MAILBOX_KHR";
		break;
	case VK_PRESENT_MODE_FIFO_KHR:
		std::cout << "VK_PRESENT_MODE_FIFO_KHR";
		break;
	case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
		std::cout << "VK_PRESENT_MODE_FIFO_RELAXED_KHR";
		break;
	default:
		std::cout << "unknown";
	}
	std::cout << '\n';
}

template <>
void print_vulkan_struct(VkSurfaceCapabilitiesKHR info)
{
	std::cout << "minImageCount: " << info.minImageCount << '\n';
	std::cout << "maxImageCount: " << info.maxImageCount << '\n';
	print_vulkan_extent("currentExtent", info.currentExtent);
	print_vulkan_extent("minImageExtent", info.minImageExtent);
	print_vulkan_extent("maxImageExtent", info.maxImageExtent);
	std::cout << "maxImageArrayLayers: " << info.maxImageArrayLayers << '\n';
	std::cout << "supportedTransforms: " << info.supportedTransforms << '\n';
	std::cout << "currentTransform: " << info.currentTransform << '\n';
	std::cout << "supportedCompositeAlpha: " << info.supportedCompositeAlpha << '\n';
	std::cout << "supportedUsageFlags: " << info.supportedUsageFlags << '\n';
}

}
