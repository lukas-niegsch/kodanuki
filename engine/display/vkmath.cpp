#include "engine/display/vkmath.h"


namespace kodanuki::vkmath
{

VulkanTensor empty_tensor_copy(VulkanTensor tensorA)
{
	VulkanTensor tensorZ = vkinit::tensor({
		.shape        = tensorA.shape,
		.element_size = tensorA.element_size,
		.usage        = tensorA.usage_flags,
	}, tensorA.device);
	return tensorZ;
}

std::string get_compute_path(std::string shader, bool is_constant, bool is_inplace)
{
	std::string prefix = "assets/shaders/vt_op_";
	std::string suffix = ".comp.spv";
	std::string middle = "";
	if (is_inplace) {
		middle += "i";
	}
	if (is_constant) {
		middle += "c";
	}
	if (!middle.empty()) {
		middle = std::string("_") + middle;
	}
	return prefix + shader + middle + suffix;
}

VulkanTensor execute(
	std::string               shader,
	std::vector<VulkanTensor> tensors,
	std::vector<float>        constants,
	bool                      is_constant,
	bool                      is_inplace)
{
	uint32_t element_count = tensors[0].element_count;
	uint32_t dimension_count = tensors[0].shape.size();
	constants.push_back(std::bit_cast<float>(element_count));
	constants.push_back(std::bit_cast<float>(dimension_count));
	for (uint32_t dimension : tensors[0].shape) {
		constants.push_back(std::bit_cast<float>(dimension));
	}
	if (!is_inplace) {
		tensors.insert(tensors.begin(), empty_tensor_copy(tensors[0]));
	}
	std::string path_shader = get_compute_path(shader, is_constant, is_inplace);
	execute_compute_shader(path_shader, {tensors}, {constants});
	return tensors[0];
}

VulkanTensor id(VulkanTensor tensorA, bool inplace)
{
	return execute("copy", {tensorA}, {}, false, inplace);
}

VulkanTensor abs(VulkanTensor tensorA, bool inplace)
{
	return execute("abs", {tensorA}, {}, false, inplace);
}

VulkanTensor sign(VulkanTensor tensorA, bool inplace)
{
	return execute("sign", {tensorA}, {}, false, inplace);
}

VulkanTensor ceil(VulkanTensor tensorA, bool inplace)
{
	return execute("ceil", {tensorA}, {}, false, inplace);
}

VulkanTensor floor(VulkanTensor tensorA, bool inplace)
{
	return execute("floor", {tensorA}, {}, false, inplace);
}

VulkanTensor round(VulkanTensor tensorA, bool inplace)
{
	return execute("round", {tensorA}, {}, false, inplace);
}

VulkanTensor clamp(VulkanTensor tensorA, VulkanTensor tensorB, VulkanTensor tensorC, bool inplace)
{
	return execute("clamp", {tensorA, tensorB, tensorC}, {}, false, inplace);
}

VulkanTensor clamp(VulkanTensor tensorA, float constB, float constC, bool inplace)
{
	return execute("clamp", {tensorA}, {constB, constC}, true, inplace);
}

VulkanTensor min(VulkanTensor tensorA, VulkanTensor tensorB, bool inplace)
{
	return execute("min", {tensorA, tensorB}, {}, false, inplace);
}

VulkanTensor min(VulkanTensor tensorA, float constB, bool inplace)
{
	return execute("min", {tensorA}, {constB}, true, inplace);
}

VulkanTensor max(VulkanTensor tensorA, VulkanTensor tensorB, bool inplace)
{
	return execute("max", {tensorA, tensorB}, {}, false, inplace);
}

VulkanTensor max(VulkanTensor tensorA, float constB, bool inplace)
{
	return execute("max", {tensorA}, {constB}, true, inplace);
}

VulkanTensor add(VulkanTensor tensorA, VulkanTensor tensorB, bool inplace)
{
	return execute("add", {tensorA, tensorB}, {}, false, inplace);
}

VulkanTensor add(VulkanTensor tensorA, float constB, bool inplace)
{
	return execute("add", {tensorA}, {constB}, true, inplace);
}

VulkanTensor sub(VulkanTensor tensorA, VulkanTensor tensorB, bool inplace)
{
	return execute("sub", {tensorA, tensorB}, {}, false, inplace);
}

VulkanTensor sub(VulkanTensor tensorA, float constB, bool inplace)
{
	return execute("sub", {tensorA}, {constB}, true, inplace);
}

VulkanTensor mul(VulkanTensor tensorA, VulkanTensor tensorB, bool inplace)
{
	return execute("mul", {tensorA, tensorB}, {}, false, inplace);
}

VulkanTensor mul(VulkanTensor tensorA, float constB, bool inplace)
{
	return execute("mul", {tensorA}, {constB}, true, inplace);
}

VulkanTensor div(VulkanTensor tensorA, VulkanTensor tensorB, bool inplace)
{
	return execute("div", {tensorA, tensorB}, {}, false, inplace);
}

VulkanTensor div(VulkanTensor tensorA, float constB, bool inplace)
{
	return execute("div", {tensorA}, {constB}, true, inplace);
}

VulkanTensor linear(VulkanTensor tensorA, VulkanTensor tensorB, float alpha, float beta, bool inplace)
{
	return execute("linear", {tensorA, tensorB}, {alpha, beta}, false, inplace);
}

VulkanTensor linear(VulkanTensor tensorA, float alpha, float beta, bool inplace)
{
	return execute("linear", {tensorA}, {alpha, beta, 1.0f}, true, inplace);
}

VulkanTensor eq(VulkanTensor tensorA, VulkanTensor tensorB, bool inplace)
{
	return execute("eq", {tensorA, tensorB}, {}, false, inplace);
}

VulkanTensor eq(VulkanTensor tensorA, float constB, bool inplace)
{
	return execute("eq", {tensorA}, {constB}, true, inplace);
}

VulkanTensor gt(VulkanTensor tensorA, VulkanTensor tensorB, bool inplace)
{
	return execute("gt", {tensorA, tensorB}, {}, false, inplace);
}

VulkanTensor gt(VulkanTensor tensorA, float constB, bool inplace)
{
	return execute("gt", {tensorA}, {constB}, true, inplace);
}

VulkanTensor geq(VulkanTensor tensorA, VulkanTensor tensorB, bool inplace)
{
	return execute("geq", {tensorA, tensorB}, {}, false, inplace);
}

VulkanTensor geq(VulkanTensor tensorA, float constB, bool inplace)
{
	return execute("geq", {tensorA}, {constB}, true, inplace);
}

VulkanTensor lt(VulkanTensor tensorA, VulkanTensor tensorB, bool inplace)
{
	return execute("lt", {tensorA, tensorB}, {}, false, inplace);
}

VulkanTensor lt(VulkanTensor tensorA, float constB, bool inplace)
{
	return execute("lt", {tensorA}, {constB}, true, inplace);
}

VulkanTensor leq(VulkanTensor tensorA, VulkanTensor tensorB, bool inplace)
{
	return execute("leq", {tensorA, tensorB}, {}, false, inplace);
}

VulkanTensor leq(VulkanTensor tensorA, float constB, bool inplace)
{
	return execute("leq", {tensorA}, {constB}, true, inplace);
}

VulkanTensor sin(VulkanTensor tensorA, bool inplace)
{
	return execute("sin", {tensorA}, {}, false, inplace);
}

VulkanTensor cos(VulkanTensor tensorA, bool inplace)
{
	return execute("cos", {tensorA}, {}, false, inplace);
}

VulkanTensor tan(VulkanTensor tensorA, bool inplace)
{
	return execute("tan", {tensorA}, {}, false, inplace);
}

VulkanTensor exp(VulkanTensor tensorA, bool inplace)
{
	return execute("exp", {tensorA}, {}, false, inplace);
}

VulkanTensor log(VulkanTensor tensorA, bool inplace)
{
	return execute("log", {tensorA}, {}, false, inplace);
}

VulkanTensor pow(VulkanTensor tensorA, VulkanTensor tensorB, bool inplace)
{
	return execute("pow", {tensorA, tensorB}, {}, false, inplace);
}

VulkanTensor pow(VulkanTensor tensorA, float constB, bool inplace)
{
	return execute("pow", {tensorA}, {constB}, true, inplace);
}

VulkanTensor range(VulkanTensor tensorA, float start, float step)
{
	return execute("range", {tensorA}, {start, step}, false, false);
}

VulkanTensor fill(VulkanTensor tensorA, float constB)
{
	return execute("fill", {tensorA}, {constB}, false, false);
}

}
