#include "engine/vulkan/debug.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#include "extern/SPIRV-Reflect/spirv_reflect.c"
#pragma GCC diagnostic pop

namespace kodanuki
{

void append_version(std::stringstream& ss, std::string name, uint32_t version)
{
	ss << name << ": ";
	ss << VK_VERSION_MAJOR(version) << ".";
	ss << VK_VERSION_MINOR(version) << ".";
	ss << VK_VERSION_PATCH(version) << "\n";
}

void append_extent(std::stringstream& ss, std::string name, VkExtent2D extent)
{
	ss << name << ": " << extent.width << ' ' << extent.height << '\n';
}

template <>
std::string vulkan_debug(VkResult info)
{
	std::stringstream ss;

	switch (info)
	{
	case VK_SUCCESS:
		ss << "VK_SUCCESS" << '\n';
		break;
	case VK_NOT_READY:
		ss << "VK_NOT_READY" << '\n';
		break;
	case VK_TIMEOUT:
		ss << "VK_TIMEOUT" << '\n';
		break;
	case VK_EVENT_SET:
		ss << "VK_EVENT_SET" << '\n';
		break;
	case VK_EVENT_RESET:
		ss << "VK_EVENT_RESET" << '\n';
		break;
	case VK_INCOMPLETE:
		ss << "VK_INCOMPLETE" << '\n';
		break;
	case VK_ERROR_OUT_OF_HOST_MEMORY:
		ss << "VK_ERROR_OUT_OF_HOST_MEMORY" << '\n';
		break;
	case VK_ERROR_OUT_OF_DEVICE_MEMORY:
		ss << "VK_ERROR_OUT_OF_DEVICE_MEMORY" << '\n';
		break;
	case VK_ERROR_DEVICE_LOST:
		ss << "VK_ERROR_DEVICE_LOST" << '\n';
		break;
	case VK_ERROR_MEMORY_MAP_FAILED:
		ss << "VK_ERROR_MEMORY_MAP_FAILED" << '\n';
		break;
	case VK_ERROR_LAYER_NOT_PRESENT:
		ss << "VK_ERROR_LAYER_NOT_PRESENT" << '\n';
		break;
	case VK_ERROR_EXTENSION_NOT_PRESENT:
		ss << "VK_ERROR_EXTENSION_NOT_PRESENT" << '\n';
		break;
	case VK_ERROR_FEATURE_NOT_PRESENT:
		ss << "VK_ERROR_FEATURE_NOT_PRESENT" << '\n';
		break;
	case VK_ERROR_INCOMPATIBLE_DRIVER:
		ss << "VK_ERROR_INCOMPATIBLE_DRIVER" << '\n';
		break;
	case VK_ERROR_TOO_MANY_OBJECTS:
		ss << "VK_ERROR_TOO_MANY_OBJECTS" << '\n';
		break;
	case VK_ERROR_FORMAT_NOT_SUPPORTED:
		ss << "VK_ERROR_FORMAT_NOT_SUPPORTED" << '\n';
		break;
	case VK_ERROR_FRAGMENTED_POOL:
		ss << "VK_ERROR_FRAGMENTED_POOL" << '\n';
		break;
	case VK_ERROR_UNKNOWN:
		ss << "VK_ERROR_UNKNOWN" << '\n';
		break;
	case VK_ERROR_OUT_OF_POOL_MEMORY:
		ss << "VK_ERROR_OUT_OF_POOL_MEMORY" << '\n';
		break;
	case VK_ERROR_INVALID_EXTERNAL_HANDLE:
		ss << "VK_ERROR_INVALID_EXTERNAL_HANDLE" << '\n';
		break;
	case VK_ERROR_FRAGMENTATION:
		ss << "VK_ERROR_FRAGMENTATION" << '\n';
		break;
	case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
		ss << "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS" << '\n';
		break;
	case VK_PIPELINE_COMPILE_REQUIRED:
		ss << "VK_PIPELINE_COMPILE_REQUIRED" << '\n';
		break;
	default:
		ss << "ERROR CODE NOT LISTED: " << info << '\n';
	}

	return ss.str();
}

template <>
std::string vulkan_debug(SpvReflectResult info)
{
	std::stringstream ss;

	switch (info)
	{
	case SPV_REFLECT_RESULT_SUCCESS:
		ss << "SPV_REFLECT_RESULT_SUCCESS" << '\n';
		break;
	case SPV_REFLECT_RESULT_NOT_READY:
		ss << "SPV_REFLECT_RESULT_NOT_READY" << '\n';
		break;
	case SPV_REFLECT_RESULT_ERROR_PARSE_FAILED:
		ss << "SPV_REFLECT_RESULT_ERROR_PARSE_FAILED" << '\n';
		break;
	case SPV_REFLECT_RESULT_ERROR_ALLOC_FAILED:
		ss << "SPV_REFLECT_RESULT_ERROR_ALLOC_FAILED" << '\n';
		break;
	case SPV_REFLECT_RESULT_ERROR_RANGE_EXCEEDED:
		ss << "SPV_REFLECT_RESULT_ERROR_RANGE_EXCEEDED" << '\n';
		break;
	case SPV_REFLECT_RESULT_ERROR_NULL_POINTER:
		ss << "SPV_REFLECT_RESULT_ERROR_NULL_POINTER" << '\n';
		break;
	case SPV_REFLECT_RESULT_ERROR_INTERNAL_ERROR:
		ss << "SPV_REFLECT_RESULT_ERROR_INTERNAL_ERROR" << '\n';
		break;
	case SPV_REFLECT_RESULT_ERROR_COUNT_MISMATCH:
		ss << "SPV_REFLECT_RESULT_ERROR_COUNT_MISMATCH" << '\n';
		break;
	case SPV_REFLECT_RESULT_ERROR_ELEMENT_NOT_FOUND:
		ss << "SPV_REFLECT_RESULT_ERROR_ELEMENT_NOT_FOUND" << '\n';
		break;
	case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_CODE_SIZE:
		ss << "SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_CODE_SIZE" << '\n';
		break;
	case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_MAGIC_NUMBER:
		ss << "SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_MAGIC_NUMBER" << '\n';
		break;
	case SPV_REFLECT_RESULT_ERROR_SPIRV_UNEXPECTED_EOF:
		ss << "SPV_REFLECT_RESULT_ERROR_SPIRV_UNEXPECTED_EOF" << '\n';
		break;
	case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_ID_REFERENCE:
		ss << "SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_ID_REFERENCE" << '\n';
		break;
	case SPV_REFLECT_RESULT_ERROR_SPIRV_SET_NUMBER_OVERFLOW:
		ss << "SPV_REFLECT_RESULT_ERROR_SPIRV_SET_NUMBER_OVERFLOW" << '\n';
		break;
	case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_STORAGE_CLASS:
		ss << "SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_STORAGE_CLASS" << '\n';
		break;
	case SPV_REFLECT_RESULT_ERROR_SPIRV_RECURSION:
		ss << "SPV_REFLECT_RESULT_ERROR_SPIRV_RECURSION" << '\n';
		break;
	case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_INSTRUCTION:
		ss << "SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_INSTRUCTION" << '\n';
		break;
	case SPV_REFLECT_RESULT_ERROR_SPIRV_UNEXPECTED_BLOCK_DATA:
		ss << "SPV_REFLECT_RESULT_ERROR_SPIRV_UNEXPECTED_BLOCK_DATA" << '\n';
		break;
	case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_BLOCK_MEMBER_REFERENCE:
		ss << "SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_BLOCK_MEMBER_REFERENCE" << '\n';
		break;
	case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_ENTRY_POINT:
		ss << "SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_ENTRY_POINT" << '\n';
		break;
	case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_EXECUTION_MODE:
		ss << "SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_EXECUTION_MODE" << '\n';
		break;
	default:
		ss << "ERROR CODE NOT LISTED: " << info << '\n';
	}

	return ss.str();
}


template <>
std::string vulkan_debug(VkExtensionProperties info)
{
	std::stringstream ss;
	ss << "extensionName: " << info.extensionName << '\n';
	append_version(ss, "specVersion", info.specVersion);
	return ss.str();
}

template <>
std::string vulkan_debug(VkLayerProperties info)
{
	std::stringstream ss;
	ss << "layerName: " << info.layerName << '\n';
	append_version(ss, "specVersion", info.specVersion);
	append_version(ss, "implementationVersion", info.implementationVersion);
	ss << "description: " << info.description << '\n';
	return ss.str();
}

template <>
std::string vulkan_debug(VkPhysicalDeviceProperties info)
{
	std::stringstream ss;
	append_version(ss, "apiVersion", info.apiVersion);
	append_version(ss, "driverVersion", info.driverVersion);
	ss << "vendorID: " << info.vendorID << '\n';
	ss << "deviceID: " << info.deviceID << '\n';
	ss << "deviceType: ";
	switch(info.deviceType)
	{
	case VK_PHYSICAL_DEVICE_TYPE_OTHER:
		ss << "VK_PHYSICAL_DEVICE_TYPE_OTHER";
		break;
	case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
		ss << "VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU";
		break;
	case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
		ss << "VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU";
		break;
	case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
		ss << "VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU";
		break;
	case VK_PHYSICAL_DEVICE_TYPE_CPU:
		ss << "VK_PHYSICAL_DEVICE_TYPE_CPU";
		break;
	default:
		ss << "unknown";
	}
	ss << '\n';
	ss << "deviceName: " << info.deviceName << '\n';

	ss << "pipelineCacheUUID:";
	for (uint32_t i = 0; i < VK_UUID_SIZE; i++)
	{
		ss << " " << +info.pipelineCacheUUID[i];
	}
	ss << '\n';
	return ss.str();
}

template <>
std::string vulkan_debug(VkQueueFamilyProperties info)
{
	std::stringstream ss;

	#define PRINT_ENUM_FLAG(name) \
		ss << #name << ": " << (info.queueFlags & name ? "true" : "false") << '\n'

	ss << "queueFlags: " << '\n';
	PRINT_ENUM_FLAG(VK_QUEUE_GRAPHICS_BIT);
	PRINT_ENUM_FLAG(VK_QUEUE_COMPUTE_BIT);
	PRINT_ENUM_FLAG(VK_QUEUE_TRANSFER_BIT);
	PRINT_ENUM_FLAG(VK_QUEUE_SPARSE_BINDING_BIT);
	PRINT_ENUM_FLAG(VK_QUEUE_PROTECTED_BIT);

	#undef PRINT_ENUM_FLAG

	ss << "queueCount: " << info.queueCount << '\n';
	ss << "timestampValidBits: " << info.timestampValidBits << '\n';
	ss << "minImageTransferGranularity: ";
	ss << info.minImageTransferGranularity.width << "x";
	ss << info.minImageTransferGranularity.height << "x";
	ss << info.minImageTransferGranularity.depth << '\n';
	return ss.str();
}

template <>
std::string vulkan_debug(VkPhysicalDeviceFeatures info)
{
	std::stringstream ss;

	#define PRINT_VULKAN_BOOL(name) \
		ss << #name << ": " << (info.name ? "true" : "false") << '\n'

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
	return ss.str();
}

template <>
std::string vulkan_debug(VkSurfaceFormatKHR info)
{
	std::stringstream ss;
	ss << "format: " << info.format << '\n';
	ss << "colorSpace: " << info.colorSpace << '\n';
	return ss.str();
}

template <>
std::string vulkan_debug(VkPresentModeKHR info)
{
	std::stringstream ss;
	ss << "mode: ";
	switch(info)
	{
	case VK_PRESENT_MODE_IMMEDIATE_KHR:
		ss << "VK_PRESENT_MODE_IMMEDIATE_KHR";
		break;
	case VK_PRESENT_MODE_MAILBOX_KHR:
		ss << "VK_PRESENT_MODE_MAILBOX_KHR";
		break;
	case VK_PRESENT_MODE_FIFO_KHR:
		ss << "VK_PRESENT_MODE_FIFO_KHR";
		break;
	case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
		ss << "VK_PRESENT_MODE_FIFO_RELAXED_KHR";
		break;
	default:
		ss << "unknown";
	}
	ss << '\n';
	return ss.str();
}

template <>
std::string vulkan_debug(VkSurfaceCapabilitiesKHR info)
{
	std::stringstream ss;
	ss << "minImageCount: " << info.minImageCount << '\n';
	ss << "maxImageCount: " << info.maxImageCount << '\n';
	append_extent(ss, "currentExtent", info.currentExtent);
	append_extent(ss, "minImageExtent", info.minImageExtent);
	append_extent(ss, "maxImageExtent", info.maxImageExtent);
	ss << "maxImageArrayLayers: " << info.maxImageArrayLayers << '\n';
	ss << "supportedTransforms: " << info.supportedTransforms << '\n';
	ss << "currentTransform: " << info.currentTransform << '\n';
	ss << "supportedCompositeAlpha: " << info.supportedCompositeAlpha << '\n';
	ss << "supportedUsageFlags: " << info.supportedUsageFlags << '\n';
	return ss.str();
}

template <>
std::string vulkan_debug(VkPhysicalDeviceMemoryProperties info)
{
	std::stringstream ss;
	ss << "memoryTypeCount: " << info.memoryTypeCount << '\n';
	std::vector<VkMemoryType> types(info.memoryTypes, info.memoryTypes + info.memoryTypeCount);
	ss << vulkan_debug(types);
	ss << std::string(LINE_LENGTH, '-') << '\n';
	ss << "memoryHeapCount: " << info.memoryHeapCount << '\n';
	std::vector<VkMemoryHeap> heaps(info.memoryHeaps, info.memoryHeaps + info.memoryHeapCount);
	ss << vulkan_debug(heaps);
	return ss.str();
}

template <>
std::string vulkan_debug(VkMemoryHeap info)
{
	std::stringstream ss;
	ss << "size: " << info.size / (1024 * 1024) << " MiB" << '\n';
	ss << "flags: " << info.flags << '\n';
	if (info.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) ss << "VK_MEMORY_HEAP_DEVICE_LOCAL_BIT" << '\n';
	if (info.flags & VK_MEMORY_HEAP_MULTI_INSTANCE_BIT) ss << "VK_MEMORY_HEAP_MULTI_INSTANCE_BIT" << '\n';
	return ss.str();
}

template <>
std::string vulkan_debug(VkMemoryType info)
{
	std::stringstream ss;
	ss << "heapIndex: " << info.heapIndex << '\n';
	ss << "propertyFlags: " << info.propertyFlags << '\n';
	if (info.propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) ss << "VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT" << '\n';
	if (info.propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) ss << "VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT" << '\n';
	if (info.propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) ss << "VK_MEMORY_PROPERTY_HOST_COHERENT_BIT" << '\n';
	if (info.propertyFlags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT) ss << "VK_MEMORY_PROPERTY_HOST_CACHED_BIT" << '\n';
	if (info.propertyFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT) ss << "VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT" << '\n';
	if (info.propertyFlags & VK_MEMORY_PROPERTY_PROTECTED_BIT) ss << "VK_MEMORY_PROPERTY_PROTECTED_BIT" << '\n';
	return ss.str();
}

template <>
std::string vulkan_debug(VkFormat info)
{
	std::stringstream ss;

	switch (info)
	{
	case VK_FORMAT_UNDEFINED:
		ss << "VK_FORMAT_UNDEFINED";
		break;
	case VK_FORMAT_R4G4_UNORM_PACK8:
		ss << "VK_FORMAT_R4G4_UNORM_PACK8";
		break;
	case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
		ss << "VK_FORMAT_R4G4B4A4_UNORM_PACK16";
		break;
	case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
		ss << "VK_FORMAT_B4G4R4A4_UNORM_PACK16";
		break;
	case VK_FORMAT_R5G6B5_UNORM_PACK16:
		ss << "VK_FORMAT_R5G6B5_UNORM_PACK16";
		break;
	case VK_FORMAT_B5G6R5_UNORM_PACK16:
		ss << "VK_FORMAT_B5G6R5_UNORM_PACK16";
		break;
	case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
		ss << "VK_FORMAT_R5G5B5A1_UNORM_PACK16";
		break;
	case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
		ss << "VK_FORMAT_B5G5R5A1_UNORM_PACK16";
		break;
	case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
		ss << "VK_FORMAT_A1R5G5B5_UNORM_PACK16";
		break;
	case VK_FORMAT_R8_UNORM:
		ss << "VK_FORMAT_R8_UNORM";
		break;
	case VK_FORMAT_R8_SNORM:
		ss << "VK_FORMAT_R8_SNORM";
		break;
	case VK_FORMAT_R8_USCALED:
		ss << "VK_FORMAT_R8_USCALED";
		break;
	case VK_FORMAT_R8_SSCALED:
		ss << "VK_FORMAT_R8_SSCALED";
		break;
	case VK_FORMAT_R8_UINT:
		ss << "VK_FORMAT_R8_UINT";
		break;
	case VK_FORMAT_R8_SINT:
		ss << "VK_FORMAT_R8_SINT";
		break;
	case VK_FORMAT_R8_SRGB:
		ss << "VK_FORMAT_R8_SRGB";
		break;
	case VK_FORMAT_R8G8_UNORM:
		ss << "VK_FORMAT_R8G8_UNORM";
		break;
	case VK_FORMAT_R8G8_SNORM:
		ss << "VK_FORMAT_R8G8_SNORM";
		break;
	case VK_FORMAT_R8G8_USCALED:
		ss << "VK_FORMAT_R8G8_USCALED";
		break;
	case VK_FORMAT_R8G8_SSCALED:
		ss << "VK_FORMAT_R8G8_SSCALED";
		break;
	case VK_FORMAT_R8G8_UINT:
		ss << "VK_FORMAT_R8G8_UINT";
		break;
	case VK_FORMAT_R8G8_SINT:
		ss << "VK_FORMAT_R8G8_SINT";
		break;
	case VK_FORMAT_R8G8_SRGB:
		ss << "VK_FORMAT_R8G8_SRGB";
		break;
	case VK_FORMAT_R8G8B8_UNORM:
		ss << "VK_FORMAT_R8G8B8_UNORM";
		break;
	case VK_FORMAT_R8G8B8_SNORM:
		ss << "VK_FORMAT_R8G8B8_SNORM";
		break;
	case VK_FORMAT_R8G8B8_USCALED:
		ss << "VK_FORMAT_R8G8B8_USCALED";
		break;
	case VK_FORMAT_R8G8B8_SSCALED:
		ss << "VK_FORMAT_R8G8B8_SSCALED";
		break;
	case VK_FORMAT_R8G8B8_UINT:
		ss << "VK_FORMAT_R8G8B8_UINT";
		break;
	case VK_FORMAT_R8G8B8_SINT:
		ss << "VK_FORMAT_R8G8B8_SINT";
		break;
	case VK_FORMAT_R8G8B8_SRGB:
		ss << "VK_FORMAT_R8G8B8_SRGB";
		break;
	case VK_FORMAT_B8G8R8_UNORM:
		ss << "VK_FORMAT_B8G8R8_UNORM";
		break;
	case VK_FORMAT_B8G8R8_SNORM:
		ss << "VK_FORMAT_B8G8R8_SNORM";
		break;
	case VK_FORMAT_B8G8R8_USCALED:
		ss << "VK_FORMAT_B8G8R8_USCALED";
		break;
	case VK_FORMAT_B8G8R8_SSCALED:
		ss << "VK_FORMAT_B8G8R8_SSCALED";
		break;
	case VK_FORMAT_B8G8R8_UINT:
		ss << "VK_FORMAT_B8G8R8_UINT";
		break;
	case VK_FORMAT_B8G8R8_SINT:
		ss << "VK_FORMAT_B8G8R8_SINT";
		break;
	case VK_FORMAT_B8G8R8_SRGB:
		ss << "VK_FORMAT_B8G8R8_SRGB";
		break;
	case VK_FORMAT_R8G8B8A8_UNORM:
		ss << "VK_FORMAT_R8G8B8A8_UNORM";
		break;
	case VK_FORMAT_R8G8B8A8_SNORM:
		ss << "VK_FORMAT_R8G8B8A8_SNORM";
		break;
	case VK_FORMAT_R8G8B8A8_USCALED:
		ss << "VK_FORMAT_R8G8B8A8_USCALED";
		break;
	case VK_FORMAT_R8G8B8A8_SSCALED:
		ss << "VK_FORMAT_R8G8B8A8_SSCALED";
		break;
	case VK_FORMAT_R8G8B8A8_UINT:
		ss << "VK_FORMAT_R8G8B8A8_UINT";
		break;
	case VK_FORMAT_R8G8B8A8_SINT:
		ss << "VK_FORMAT_R8G8B8A8_SINT";
		break;
	case VK_FORMAT_R8G8B8A8_SRGB:
		ss << "VK_FORMAT_R8G8B8A8_SRGB";
		break;
	case VK_FORMAT_B8G8R8A8_UNORM:
		ss << "VK_FORMAT_B8G8R8A8_UNORM";
		break;
	case VK_FORMAT_B8G8R8A8_SNORM:
		ss << "VK_FORMAT_B8G8R8A8_SNORM";
		break;
	case VK_FORMAT_B8G8R8A8_USCALED:
		ss << "VK_FORMAT_B8G8R8A8_USCALED";
		break;
	case VK_FORMAT_B8G8R8A8_SSCALED:
		ss << "VK_FORMAT_B8G8R8A8_SSCALED";
		break;
	case VK_FORMAT_B8G8R8A8_UINT:
		ss << "VK_FORMAT_B8G8R8A8_UINT";
		break;
	case VK_FORMAT_B8G8R8A8_SINT:
		ss << "VK_FORMAT_B8G8R8A8_SINT";
		break;
	case VK_FORMAT_B8G8R8A8_SRGB:
		ss << "VK_FORMAT_B8G8R8A8_SRGB";
		break;
	case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
		ss << "VK_FORMAT_A8B8G8R8_UNORM_PACK32";
		break;
	case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
		ss << "VK_FORMAT_A8B8G8R8_SNORM_PACK32";
		break;
	case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
		ss << "VK_FORMAT_A8B8G8R8_USCALED_PACK32";
		break;
	case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
		ss << "VK_FORMAT_A8B8G8R8_SSCALED_PACK32";
		break;
	case VK_FORMAT_A8B8G8R8_UINT_PACK32:
		ss << "VK_FORMAT_A8B8G8R8_UINT_PACK32";
		break;
	case VK_FORMAT_A8B8G8R8_SINT_PACK32:
		ss << "VK_FORMAT_A8B8G8R8_SINT_PACK32";
		break;
	case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
		ss << "VK_FORMAT_A8B8G8R8_SRGB_PACK32";
		break;
	case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
		ss << "VK_FORMAT_A2R10G10B10_UNORM_PACK32";
		break;
	case VK_FORMAT_A2R10G10B10_SNORM_PACK32:
		ss << "VK_FORMAT_A2R10G10B10_SNORM_PACK32";
		break;
	case VK_FORMAT_A2R10G10B10_USCALED_PACK32:
		ss << "VK_FORMAT_A2R10G10B10_USCALED_PACK32";
		break;
	case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:
		ss << "VK_FORMAT_A2R10G10B10_SSCALED_PACK32";
		break;
	case VK_FORMAT_A2R10G10B10_UINT_PACK32:
		ss << "VK_FORMAT_A2R10G10B10_UINT_PACK32";
		break;
	case VK_FORMAT_A2R10G10B10_SINT_PACK32:
		ss << "VK_FORMAT_A2R10G10B10_SINT_PACK32";
		break;
	case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
		ss << "VK_FORMAT_A2B10G10R10_UNORM_PACK32";
		break;
	case VK_FORMAT_A2B10G10R10_SNORM_PACK32:
		ss << "VK_FORMAT_A2B10G10R10_SNORM_PACK32";
		break;
	case VK_FORMAT_A2B10G10R10_USCALED_PACK32:
		ss << "VK_FORMAT_A2B10G10R10_USCALED_PACK32";
		break;
	case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:
		ss << "VK_FORMAT_A2B10G10R10_SSCALED_PACK32";
		break;
	case VK_FORMAT_A2B10G10R10_UINT_PACK32:
		ss << "VK_FORMAT_A2B10G10R10_UINT_PACK32";
		break;
	case VK_FORMAT_A2B10G10R10_SINT_PACK32:
		ss << "VK_FORMAT_A2B10G10R10_SINT_PACK32";
		break;
	case VK_FORMAT_R16_UNORM:
		ss << "VK_FORMAT_R16_UNORM";
		break;
	case VK_FORMAT_R16_SNORM:
		ss << "VK_FORMAT_R16_SNORM";
		break;
	case VK_FORMAT_R16_USCALED:
		ss << "VK_FORMAT_R16_USCALED";
		break;
	case VK_FORMAT_R16_SSCALED:
		ss << "VK_FORMAT_R16_SSCALED";
		break;
	case VK_FORMAT_R16_UINT:
		ss << "VK_FORMAT_R16_UINT";
		break;
	case VK_FORMAT_R16_SINT:
		ss << "VK_FORMAT_R16_SINT";
		break;
	case VK_FORMAT_R16_SFLOAT:
		ss << "VK_FORMAT_R16_SFLOAT";
		break;
	case VK_FORMAT_R16G16_UNORM:
		ss << "VK_FORMAT_R16G16_UNORM";
		break;
	case VK_FORMAT_R16G16_SNORM:
		ss << "VK_FORMAT_R16G16_SNORM";
		break;
	case VK_FORMAT_R16G16_USCALED:
		ss << "VK_FORMAT_R16G16_USCALED";
		break;
	case VK_FORMAT_R16G16_SSCALED:
		ss << "VK_FORMAT_R16G16_SSCALED";
		break;
	case VK_FORMAT_R16G16_UINT:
		ss << "VK_FORMAT_R16G16_UINT";
		break;
	case VK_FORMAT_R16G16_SINT:
		ss << "VK_FORMAT_R16G16_SINT";
		break;
	case VK_FORMAT_R16G16_SFLOAT:
		ss << "VK_FORMAT_R16G16_SFLOAT";
		break;
	case VK_FORMAT_R16G16B16_UNORM:
		ss << "VK_FORMAT_R16G16B16_UNORM";
		break;
	case VK_FORMAT_R16G16B16_SNORM:
		ss << "VK_FORMAT_R16G16B16_SNORM";
		break;
	case VK_FORMAT_R16G16B16_USCALED:
		ss << "VK_FORMAT_R16G16B16_USCALED";
		break;
	case VK_FORMAT_R16G16B16_SSCALED:
		ss << "VK_FORMAT_R16G16B16_SSCALED";
		break;
	case VK_FORMAT_R16G16B16_UINT:
		ss << "VK_FORMAT_R16G16B16_UINT";
		break;
	case VK_FORMAT_R16G16B16_SINT:
		ss << "VK_FORMAT_R16G16B16_SINT";
		break;
	case VK_FORMAT_R16G16B16_SFLOAT:
		ss << "VK_FORMAT_R16G16B16_SFLOAT";
		break;
	case VK_FORMAT_R16G16B16A16_UNORM:
		ss << "VK_FORMAT_R16G16B16A16_UNORM";
		break;
	case VK_FORMAT_R16G16B16A16_SNORM:
		ss << "VK_FORMAT_R16G16B16A16_SNORM";
		break;
	case VK_FORMAT_R16G16B16A16_USCALED:
		ss << "VK_FORMAT_R16G16B16A16_USCALED";
		break;
	case VK_FORMAT_R16G16B16A16_SSCALED:
		ss << "VK_FORMAT_R16G16B16A16_SSCALED";
		break;
	case VK_FORMAT_R16G16B16A16_UINT:
		ss << "VK_FORMAT_R16G16B16A16_UINT";
		break;
	case VK_FORMAT_R16G16B16A16_SINT:
		ss << "VK_FORMAT_R16G16B16A16_SINT";
		break;
	case VK_FORMAT_R16G16B16A16_SFLOAT:
		ss << "VK_FORMAT_R16G16B16A16_SFLOAT";
		break;
	case VK_FORMAT_R32_UINT:
		ss << "VK_FORMAT_R32_UINT";
		break;
	case VK_FORMAT_R32_SINT:
		ss << "VK_FORMAT_R32_SINT";
		break;
	case VK_FORMAT_R32_SFLOAT:
		ss << "VK_FORMAT_R32_SFLOAT";
		break;
	case VK_FORMAT_R32G32_UINT:
		ss << "VK_FORMAT_R32G32_UINT";
		break;
	case VK_FORMAT_R32G32_SINT:
		ss << "VK_FORMAT_R32G32_SINT";
		break;
	case VK_FORMAT_R32G32_SFLOAT:
		ss << "VK_FORMAT_R32G32_SFLOAT";
		break;
	case VK_FORMAT_R32G32B32_UINT:
		ss << "VK_FORMAT_R32G32B32_UINT";
		break;
	case VK_FORMAT_R32G32B32_SINT:
		ss << "VK_FORMAT_R32G32B32_SINT";
		break;
	case VK_FORMAT_R32G32B32_SFLOAT:
		ss << "VK_FORMAT_R32G32B32_SFLOAT";
		break;
	case VK_FORMAT_R32G32B32A32_UINT:
		ss << "VK_FORMAT_R32G32B32A32_UINT";
		break;
	case VK_FORMAT_R32G32B32A32_SINT:
		ss << "VK_FORMAT_R32G32B32A32_SINT";
		break;
	case VK_FORMAT_R32G32B32A32_SFLOAT:
		ss << "VK_FORMAT_R32G32B32A32_SFLOAT";
		break;
	case VK_FORMAT_R64_UINT:
		ss << "VK_FORMAT_R64_UINT";
		break;
	case VK_FORMAT_R64_SINT:
		ss << "VK_FORMAT_R64_SINT";
		break;
	case VK_FORMAT_R64_SFLOAT:
		ss << "VK_FORMAT_R64_SFLOAT";
		break;
	case VK_FORMAT_R64G64_UINT:
		ss << "VK_FORMAT_R64G64_UINT";
		break;
	case VK_FORMAT_R64G64_SINT:
		ss << "VK_FORMAT_R64G64_SINT";
		break;
	case VK_FORMAT_R64G64_SFLOAT:
		ss << "VK_FORMAT_R64G64_SFLOAT";
		break;
	case VK_FORMAT_R64G64B64_UINT:
		ss << "VK_FORMAT_R64G64B64_UINT";
		break;
	case VK_FORMAT_R64G64B64_SINT:
		ss << "VK_FORMAT_R64G64B64_SINT";
		break;
	case VK_FORMAT_R64G64B64_SFLOAT:
		ss << "VK_FORMAT_R64G64B64_SFLOAT";
		break;
	case VK_FORMAT_R64G64B64A64_UINT:
		ss << "VK_FORMAT_R64G64B64A64_UINT";
		break;
	case VK_FORMAT_R64G64B64A64_SINT:
		ss << "VK_FORMAT_R64G64B64A64_SINT";
		break;
	case VK_FORMAT_R64G64B64A64_SFLOAT:
		ss << "VK_FORMAT_R64G64B64A64_SFLOAT";
		break;
	case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
		ss << "VK_FORMAT_B10G11R11_UFLOAT_PACK32";
		break;
	case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
		ss << "VK_FORMAT_E5B9G9R9_UFLOAT_PACK32";
		break;
	case VK_FORMAT_D16_UNORM:
		ss << "VK_FORMAT_D16_UNORM";
		break;
	case VK_FORMAT_X8_D24_UNORM_PACK32:
		ss << "VK_FORMAT_X8_D24_UNORM_PACK32";
		break;
	case VK_FORMAT_D32_SFLOAT:
		ss << "VK_FORMAT_D32_SFLOAT";
		break;
	case VK_FORMAT_S8_UINT:
		ss << "VK_FORMAT_S8_UINT";
		break;
	case VK_FORMAT_D16_UNORM_S8_UINT:
		ss << "VK_FORMAT_D16_UNORM_S8_UINT";
		break;
	case VK_FORMAT_D24_UNORM_S8_UINT:
		ss << "VK_FORMAT_D24_UNORM_S8_UINT";
		break;
	case VK_FORMAT_D32_SFLOAT_S8_UINT:
		ss << "VK_FORMAT_D32_SFLOAT_S8_UINT";
		break;
	case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
		ss << "VK_FORMAT_BC1_RGB_UNORM_BLOCK";
		break;
	case VK_FORMAT_BC1_RGB_SRGB_BLOCK:
		ss << "VK_FORMAT_BC1_RGB_SRGB_BLOCK";
		break;
	case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
		ss << "VK_FORMAT_BC1_RGBA_UNORM_BLOCK";
		break;
	case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
		ss << "VK_FORMAT_BC1_RGBA_SRGB_BLOCK";
		break;
	case VK_FORMAT_BC2_UNORM_BLOCK:
		ss << "VK_FORMAT_BC2_UNORM_BLOCK";
		break;
	case VK_FORMAT_BC2_SRGB_BLOCK:
		ss << "VK_FORMAT_BC2_SRGB_BLOCK";
		break;
	case VK_FORMAT_BC3_UNORM_BLOCK:
		ss << "VK_FORMAT_BC3_UNORM_BLOCK";
		break;
	case VK_FORMAT_BC3_SRGB_BLOCK:
		ss << "VK_FORMAT_BC3_SRGB_BLOCK";
		break;
	case VK_FORMAT_BC4_UNORM_BLOCK:
		ss << "VK_FORMAT_BC4_UNORM_BLOCK";
		break;
	case VK_FORMAT_BC4_SNORM_BLOCK:
		ss << "VK_FORMAT_BC4_SNORM_BLOCK";
		break;
	case VK_FORMAT_BC5_UNORM_BLOCK:
		ss << "VK_FORMAT_BC5_UNORM_BLOCK";
		break;
	case VK_FORMAT_BC5_SNORM_BLOCK:
		ss << "VK_FORMAT_BC5_SNORM_BLOCK";
		break;
	case VK_FORMAT_BC6H_UFLOAT_BLOCK:
		ss << "VK_FORMAT_BC6H_UFLOAT_BLOCK";
		break;
	case VK_FORMAT_BC6H_SFLOAT_BLOCK:
		ss << "VK_FORMAT_BC6H_SFLOAT_BLOCK";
		break;
	case VK_FORMAT_BC7_UNORM_BLOCK:
		ss << "VK_FORMAT_BC7_UNORM_BLOCK";
		break;
	case VK_FORMAT_BC7_SRGB_BLOCK:
		ss << "VK_FORMAT_BC7_SRGB_BLOCK";
		break;
	case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:
		ss << "VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK";
		break;
	case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK:
		ss << "VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK";
		break;
	case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:
		ss << "VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK";
		break;
	case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK:
		ss << "VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK";
		break;
	case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:
		ss << "VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK";
		break;
	case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK:
		ss << "VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK";
		break;
	case VK_FORMAT_EAC_R11_UNORM_BLOCK:
		ss << "VK_FORMAT_EAC_R11_UNORM_BLOCK";
		break;
	case VK_FORMAT_EAC_R11_SNORM_BLOCK:
		ss << "VK_FORMAT_EAC_R11_SNORM_BLOCK";
		break;
	case VK_FORMAT_EAC_R11G11_UNORM_BLOCK:
		ss << "VK_FORMAT_EAC_R11G11_UNORM_BLOCK";
		break;
	case VK_FORMAT_EAC_R11G11_SNORM_BLOCK:
		ss << "VK_FORMAT_EAC_R11G11_SNORM_BLOCK";
		break;
	case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:
		ss << "VK_FORMAT_ASTC_4x4_UNORM_BLOCK";
		break;
	case VK_FORMAT_ASTC_4x4_SRGB_BLOCK:
		ss << "VK_FORMAT_ASTC_4x4_SRGB_BLOCK";
		break;
	case VK_FORMAT_ASTC_5x4_UNORM_BLOCK:
		ss << "VK_FORMAT_ASTC_5x4_UNORM_BLOCK";
		break;
	case VK_FORMAT_ASTC_5x4_SRGB_BLOCK:
		ss << "VK_FORMAT_ASTC_5x4_SRGB_BLOCK";
		break;
	case VK_FORMAT_ASTC_5x5_UNORM_BLOCK:
		ss << "VK_FORMAT_ASTC_5x5_UNORM_BLOCK";
		break;
	case VK_FORMAT_ASTC_5x5_SRGB_BLOCK:
		ss << "VK_FORMAT_ASTC_5x5_SRGB_BLOCK";
		break;
	case VK_FORMAT_ASTC_6x5_UNORM_BLOCK:
		ss << "VK_FORMAT_ASTC_6x5_UNORM_BLOCK";
		break;
	case VK_FORMAT_ASTC_6x5_SRGB_BLOCK:
		ss << "VK_FORMAT_ASTC_6x5_SRGB_BLOCK";
		break;
	case VK_FORMAT_ASTC_6x6_UNORM_BLOCK:
		ss << "VK_FORMAT_ASTC_6x6_UNORM_BLOCK";
		break;
	case VK_FORMAT_ASTC_6x6_SRGB_BLOCK:
		ss << "VK_FORMAT_ASTC_6x6_SRGB_BLOCK";
		break;
	case VK_FORMAT_ASTC_8x5_UNORM_BLOCK:
		ss << "VK_FORMAT_ASTC_8x5_UNORM_BLOCK";
		break;
	case VK_FORMAT_ASTC_8x5_SRGB_BLOCK:
		ss << "VK_FORMAT_ASTC_8x5_SRGB_BLOCK";
		break;
	case VK_FORMAT_ASTC_8x6_UNORM_BLOCK:
		ss << "VK_FORMAT_ASTC_8x6_UNORM_BLOCK";
		break;
	case VK_FORMAT_ASTC_8x6_SRGB_BLOCK:
		ss << "VK_FORMAT_ASTC_8x6_SRGB_BLOCK";
		break;
	case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:
		ss << "VK_FORMAT_ASTC_8x8_UNORM_BLOCK";
		break;
	case VK_FORMAT_ASTC_8x8_SRGB_BLOCK:
		ss << "VK_FORMAT_ASTC_8x8_SRGB_BLOCK";
		break;
	case VK_FORMAT_ASTC_10x5_UNORM_BLOCK:
		ss << "VK_FORMAT_ASTC_10x5_UNORM_BLOCK";
		break;
	case VK_FORMAT_ASTC_10x5_SRGB_BLOCK:
		ss << "VK_FORMAT_ASTC_10x5_SRGB_BLOCK";
		break;
	case VK_FORMAT_ASTC_10x6_UNORM_BLOCK:
		ss << "VK_FORMAT_ASTC_10x6_UNORM_BLOCK";
		break;
	case VK_FORMAT_ASTC_10x6_SRGB_BLOCK:
		ss << "VK_FORMAT_ASTC_10x6_SRGB_BLOCK";
		break;
	case VK_FORMAT_ASTC_10x8_UNORM_BLOCK:
		ss << "VK_FORMAT_ASTC_10x8_UNORM_BLOCK";
		break;
	case VK_FORMAT_ASTC_10x8_SRGB_BLOCK:
		ss << "VK_FORMAT_ASTC_10x8_SRGB_BLOCK";
		break;
	case VK_FORMAT_ASTC_10x10_UNORM_BLOCK:
		ss << "VK_FORMAT_ASTC_10x10_UNORM_BLOCK";
		break;
	case VK_FORMAT_ASTC_10x10_SRGB_BLOCK:
		ss << "VK_FORMAT_ASTC_10x10_SRGB_BLOCK";
		break;
	case VK_FORMAT_ASTC_12x10_UNORM_BLOCK:
		ss << "VK_FORMAT_ASTC_12x10_UNORM_BLOCK";
		break;
	case VK_FORMAT_ASTC_12x10_SRGB_BLOCK:
		ss << "VK_FORMAT_ASTC_12x10_SRGB_BLOCK";
		break;
	case VK_FORMAT_ASTC_12x12_UNORM_BLOCK:
		ss << "VK_FORMAT_ASTC_12x12_UNORM_BLOCK";
		break;
	case VK_FORMAT_ASTC_12x12_SRGB_BLOCK:
		ss << "VK_FORMAT_ASTC_12x12_SRGB_BLOCK";
		break;
	default:
		ss << "unknown";
	}
	
	ss << '\n';
	return ss.str();
}

template <>
std::string vulkan_debug(VkVertexInputAttributeDescription info)
{
	std::stringstream ss;
	ss << "location: " << info.location << '\n';
	ss << "binding: " << info.binding << '\n';
	ss << "format: " << vulkan_debug(info.format);
	ss << "offset: " << info.offset << '\n';
	return ss.str();
}

template <>
std::string vulkan_debug(VkPhysicalDeviceSubgroupProperties info)
{
	std::stringstream ss;
	ss << "subgroupSize: " << info.subgroupSize << '\n';
	
	#define PRINT_ENUM_FLAG(name) \
		ss << #name << ": " << (info.supportedStages & name ? "true" : "false") << '\n'

	ss << "supportedStages: " << '\n';
	PRINT_ENUM_FLAG(VK_SHADER_STAGE_VERTEX_BIT);
	PRINT_ENUM_FLAG(VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT);
	PRINT_ENUM_FLAG(VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
	PRINT_ENUM_FLAG(VK_SHADER_STAGE_GEOMETRY_BIT);
	PRINT_ENUM_FLAG(VK_SHADER_STAGE_FRAGMENT_BIT);
	PRINT_ENUM_FLAG(VK_SHADER_STAGE_COMPUTE_BIT);
	PRINT_ENUM_FLAG(VK_SHADER_STAGE_ALL_GRAPHICS);

	#undef PRINT_ENUM_FLAG

	#define PRINT_ENUM_FLAG(name) \
		ss << #name << ": " << (info.supportedOperations & name ? "true" : "false") << '\n'

	ss << "supportedOperations: " << '\n';
	PRINT_ENUM_FLAG(VK_SUBGROUP_FEATURE_BASIC_BIT);
	PRINT_ENUM_FLAG(VK_SUBGROUP_FEATURE_VOTE_BIT);
	PRINT_ENUM_FLAG(VK_SUBGROUP_FEATURE_ARITHMETIC_BIT);
	PRINT_ENUM_FLAG(VK_SUBGROUP_FEATURE_BALLOT_BIT);
	PRINT_ENUM_FLAG(VK_SUBGROUP_FEATURE_SHUFFLE_BIT);
	PRINT_ENUM_FLAG(VK_SUBGROUP_FEATURE_SHUFFLE_RELATIVE_BIT);
	PRINT_ENUM_FLAG(VK_SUBGROUP_FEATURE_CLUSTERED_BIT);
	PRINT_ENUM_FLAG(VK_SUBGROUP_FEATURE_QUAD_BIT);

	#undef PRINT_ENUM_FLAG

	ss << "quadOperationsInAllStages: " << (info.quadOperationsInAllStages ? "true" : "false") << '\n';
	return ss.str();
}

}
