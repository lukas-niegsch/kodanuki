#include "engine/vulkan/debug.h"
#include <vulkan/vk_enum_string_helper.h>

#define LINE_LENGTH 80

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#include "extern/SPIRV-Reflect/spirv_reflect.c"
#pragma GCC diagnostic pop

#define APPEND_BOOLEAN_FIELD(name)	\
	ss << #name << ": " << (info.name ? "true" : "false") << '\n'

#define APPEND_SWITCH_CASE(name)	\
	case name:						\
		ss << #name << '\n';		\
		break

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
std::string stringify(VkBool32 info)
{
	return info ? "VK_TRUE" : "VK_FALSE";
}

template <>
std::string stringify(VkExtensionProperties info)
{
	std::stringstream ss;
	ss << "extensionName: " << info.extensionName << '\n';
	append_version(ss, "specVersion", info.specVersion);
	return ss.str();
}

template <>
std::string stringify(VkLayerProperties info)
{
	std::stringstream ss;
	ss << "layerName: " << info.layerName << '\n';
	append_version(ss, "specVersion", info.specVersion);
	append_version(ss, "implementationVersion", info.implementationVersion);
	ss << "description: " << info.description << '\n';
	return ss.str();
}

template <>
std::string stringify(VkPhysicalDeviceProperties info)
{
	std::stringstream ss;
	append_version(ss, "apiVersion", info.apiVersion);
	append_version(ss, "driverVersion", info.driverVersion);
	ss << "vendorID: " << info.vendorID << '\n';
	ss << "deviceID: " << info.deviceID << '\n';
	ss << "deviceType: " << stringify(info.deviceType);
	ss << "deviceName: " << info.deviceName << '\n';
	ss << "pipelineCacheUUID:";
	for (uint32_t i = 0; i < VK_UUID_SIZE; i++) {
		ss << " " << +info.pipelineCacheUUID[i];
	}
	ss << '\n';
	return ss.str();
}

template <>
std::string stringify(VkQueueFamilyProperties info)
{
	std::stringstream ss;
	ss << "queueFlags: " << string_VkQueueFlags(info.queueFlags) << '\n';
	ss << "queueCount: " << info.queueCount << '\n';
	ss << "timestampValidBits: " << info.timestampValidBits << '\n';
	ss << "minImageTransferGranularity: ";
	ss << info.minImageTransferGranularity.width << "x";
	ss << info.minImageTransferGranularity.height << "x";
	ss << info.minImageTransferGranularity.depth << '\n';
	return ss.str();
}

template <>
std::string stringify(VkSurfaceFormatKHR info)
{
	std::stringstream ss;
	ss << "format: " << info.format << '\n';
	ss << "colorSpace: " << info.colorSpace << '\n';
	return ss.str();
}

template <>
std::string stringify(VkSurfaceCapabilitiesKHR info)
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
std::string stringify(VkPhysicalDeviceMemoryProperties info)
{
	std::stringstream ss;
	ss << "memoryTypeCount: " << info.memoryTypeCount << '\n';
	std::vector<VkMemoryType> types(info.memoryTypes, info.memoryTypes + info.memoryTypeCount);
	ss << stringify(types);
	ss << std::string(LINE_LENGTH, '-') << '\n';
	ss << "memoryHeapCount: " << info.memoryHeapCount << '\n';
	std::vector<VkMemoryHeap> heaps(info.memoryHeaps, info.memoryHeaps + info.memoryHeapCount);
	ss << stringify(heaps);
	return ss.str();
}

template <>
std::string stringify(VkMemoryHeap info)
{
	std::stringstream ss;
	ss << "size: " << info.size / (1024 * 1024) << " MiB" << '\n';
	ss << "flags: " << string_VkMemoryHeapFlags(info.flags) << '\n';
	return ss.str();
}

template <>
std::string stringify(VkMemoryType info)
{
	std::stringstream ss;
	ss << "heapIndex: " << info.heapIndex << '\n';
	ss << "propertyFlags: " << string_VkMemoryPropertyFlags(info.propertyFlags) << '\n';
	return ss.str();
}

template <>
std::string stringify(VkVertexInputAttributeDescription info)
{
	std::stringstream ss;
	ss << "location: " << info.location << '\n';
	ss << "binding: " << info.binding << '\n';
	ss << "format: " << stringify(info.format);
	ss << "offset: " << info.offset << '\n';
	return ss.str();
}

template <>
std::string stringify(VkPhysicalDeviceSubgroupProperties info)
{
	std::stringstream ss;
	ss << "subgroupSize: " << info.subgroupSize << '\n';
	ss << "supportedStages: " << string_VkShaderStageFlags(info.supportedStages) << '\n';
	ss << "supportedOperations: " << string_VkSubgroupFeatureFlags(info.supportedOperations) << '\n';
	ss << "quadOperationsInAllStages: " << (info.quadOperationsInAllStages ? "true" : "false") << '\n';
	return ss.str();
}

template <>
std::string stringify(VkResult info)
{
	return string_VkResult(info);
}

template <>
std::string stringify(SpvReflectResult info)
{
	std::stringstream ss;
	switch (info) {
		APPEND_SWITCH_CASE(SPV_REFLECT_RESULT_SUCCESS);
		APPEND_SWITCH_CASE(SPV_REFLECT_RESULT_NOT_READY);
		APPEND_SWITCH_CASE(SPV_REFLECT_RESULT_ERROR_PARSE_FAILED);
		APPEND_SWITCH_CASE(SPV_REFLECT_RESULT_ERROR_ALLOC_FAILED);
		APPEND_SWITCH_CASE(SPV_REFLECT_RESULT_ERROR_RANGE_EXCEEDED);
		APPEND_SWITCH_CASE(SPV_REFLECT_RESULT_ERROR_NULL_POINTER);
		APPEND_SWITCH_CASE(SPV_REFLECT_RESULT_ERROR_INTERNAL_ERROR);
		APPEND_SWITCH_CASE(SPV_REFLECT_RESULT_ERROR_COUNT_MISMATCH);
		APPEND_SWITCH_CASE(SPV_REFLECT_RESULT_ERROR_ELEMENT_NOT_FOUND);
		APPEND_SWITCH_CASE(SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_CODE_SIZE);
		APPEND_SWITCH_CASE(SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_MAGIC_NUMBER);
		APPEND_SWITCH_CASE(SPV_REFLECT_RESULT_ERROR_SPIRV_UNEXPECTED_EOF);
		APPEND_SWITCH_CASE(SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_ID_REFERENCE);
		APPEND_SWITCH_CASE(SPV_REFLECT_RESULT_ERROR_SPIRV_SET_NUMBER_OVERFLOW);
		APPEND_SWITCH_CASE(SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_STORAGE_CLASS);
		APPEND_SWITCH_CASE(SPV_REFLECT_RESULT_ERROR_SPIRV_RECURSION);
		APPEND_SWITCH_CASE(SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_INSTRUCTION);
		APPEND_SWITCH_CASE(SPV_REFLECT_RESULT_ERROR_SPIRV_UNEXPECTED_BLOCK_DATA);
		APPEND_SWITCH_CASE(SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_BLOCK_MEMBER_REFERENCE);
		APPEND_SWITCH_CASE(SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_ENTRY_POINT);
		APPEND_SWITCH_CASE(SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_EXECUTION_MODE);
	default:
		ss << "ERROR CODE NOT LISTED: " << info << '\n';
	}
	return ss.str();
}

template <>
std::string stringify(VkFormat info)
{
	return string_VkFormat(info);
}

template <>
std::string stringify(VkPresentModeKHR info)
{
	return string_VkPresentModeKHR(info);
}

template <>
std::string stringify(VkPhysicalDeviceType info)
{
	return string_VkPhysicalDeviceType(info);
}

template <>
std::string stringify(VkPhysicalDeviceFeatures info)
{
	std::stringstream ss;
	APPEND_BOOLEAN_FIELD(robustBufferAccess);
	APPEND_BOOLEAN_FIELD(fullDrawIndexUint32);
	APPEND_BOOLEAN_FIELD(imageCubeArray);
	APPEND_BOOLEAN_FIELD(independentBlend);
	APPEND_BOOLEAN_FIELD(geometryShader);
	APPEND_BOOLEAN_FIELD(tessellationShader);
	APPEND_BOOLEAN_FIELD(sampleRateShading);
	APPEND_BOOLEAN_FIELD(dualSrcBlend);
	APPEND_BOOLEAN_FIELD(logicOp);
	APPEND_BOOLEAN_FIELD(multiDrawIndirect);
	APPEND_BOOLEAN_FIELD(drawIndirectFirstInstance);
	APPEND_BOOLEAN_FIELD(depthClamp);
	APPEND_BOOLEAN_FIELD(depthBiasClamp);
	APPEND_BOOLEAN_FIELD(fillModeNonSolid);
	APPEND_BOOLEAN_FIELD(depthBounds);
	APPEND_BOOLEAN_FIELD(wideLines);
	APPEND_BOOLEAN_FIELD(largePoints);
	APPEND_BOOLEAN_FIELD(alphaToOne);
	APPEND_BOOLEAN_FIELD(multiViewport);
	APPEND_BOOLEAN_FIELD(samplerAnisotropy);
	APPEND_BOOLEAN_FIELD(textureCompressionETC2);
	APPEND_BOOLEAN_FIELD(textureCompressionASTC_LDR);
	APPEND_BOOLEAN_FIELD(textureCompressionBC);
	APPEND_BOOLEAN_FIELD(occlusionQueryPrecise);
	APPEND_BOOLEAN_FIELD(pipelineStatisticsQuery);
	APPEND_BOOLEAN_FIELD(vertexPipelineStoresAndAtomics);
	APPEND_BOOLEAN_FIELD(fragmentStoresAndAtomics);
	APPEND_BOOLEAN_FIELD(shaderTessellationAndGeometryPointSize);
	APPEND_BOOLEAN_FIELD(shaderImageGatherExtended);
	APPEND_BOOLEAN_FIELD(shaderStorageImageExtendedFormats);
	APPEND_BOOLEAN_FIELD(shaderStorageImageMultisample);
	APPEND_BOOLEAN_FIELD(shaderStorageImageReadWithoutFormat);
	APPEND_BOOLEAN_FIELD(shaderStorageImageWriteWithoutFormat);
	APPEND_BOOLEAN_FIELD(shaderUniformBufferArrayDynamicIndexing);
	APPEND_BOOLEAN_FIELD(shaderSampledImageArrayDynamicIndexing);
	APPEND_BOOLEAN_FIELD(shaderStorageBufferArrayDynamicIndexing);
	APPEND_BOOLEAN_FIELD(shaderStorageImageArrayDynamicIndexing);
	APPEND_BOOLEAN_FIELD(shaderClipDistance);
	APPEND_BOOLEAN_FIELD(shaderCullDistance);
	APPEND_BOOLEAN_FIELD(shaderFloat64);
	APPEND_BOOLEAN_FIELD(shaderInt64);
	APPEND_BOOLEAN_FIELD(shaderInt16);
	APPEND_BOOLEAN_FIELD(shaderResourceResidency);
	APPEND_BOOLEAN_FIELD(shaderResourceMinLod);
	APPEND_BOOLEAN_FIELD(sparseBinding);
	APPEND_BOOLEAN_FIELD(sparseResidencyBuffer);
	APPEND_BOOLEAN_FIELD(sparseResidencyImage2D);
	APPEND_BOOLEAN_FIELD(sparseResidencyImage3D);
	APPEND_BOOLEAN_FIELD(sparseResidency2Samples);
	APPEND_BOOLEAN_FIELD(sparseResidency4Samples);
	APPEND_BOOLEAN_FIELD(sparseResidency8Samples);
	APPEND_BOOLEAN_FIELD(sparseResidency16Samples);
	APPEND_BOOLEAN_FIELD(sparseResidencyAliased);
	APPEND_BOOLEAN_FIELD(variableMultisampleRate);
	APPEND_BOOLEAN_FIELD(inheritedQueries);
	return ss.str();
}

}
