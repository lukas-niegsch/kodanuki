#include "engine/display/vkmath.h"


namespace kodanuki::vkmath
{

VulkanTensor execute(
	VulkanDevice              device,
	std::string               compute_shader,
	std::vector<VulkanTensor> tensors,
	std::vector<float>        constants,
	bool                      is_constant,
	bool                      is_inplace)
{
	// TODO: implement this method
	(void) device;
	(void) compute_shader;
	(void) tensors;
	(void) constants;
	(void) is_constant;
	(void) is_inplace;
	return {};
}

VulkanTensor id(VulkanDevice device, VulkanTensor tensorA, bool inplace)
{
	return execute(device, "copy", {tensorA}, {}, false, inplace);
}

VulkanTensor abs(VulkanDevice device, VulkanTensor tensorA, bool inplace)
{
	return execute(device, "abs", {tensorA}, {}, false, inplace);
}

VulkanTensor sign(VulkanDevice device, VulkanTensor tensorA, bool inplace)
{
	return execute(device, "sign", {tensorA}, {}, false, inplace);
}

VulkanTensor ceil(VulkanDevice device, VulkanTensor tensorA, bool inplace)
{
	return execute(device, "ceil", {tensorA}, {}, false, inplace);
}

VulkanTensor floor(VulkanDevice device, VulkanTensor tensorA, bool inplace)
{
	return execute(device, "floor", {tensorA}, {}, false, inplace);
}

VulkanTensor round(VulkanDevice device, VulkanTensor tensorA, bool inplace)
{
	return execute(device, "round", {tensorA}, {}, false, inplace);
}

VulkanTensor clamp(VulkanDevice device, VulkanTensor tensorA, VulkanTensor tensorB, VulkanTensor tensorC, bool inplace)
{
	return execute(device, "clamp", {tensorA, tensorB, tensorC}, {}, false, inplace);
}

VulkanTensor clamp(VulkanDevice device, VulkanTensor tensorA, float constB, float constC, bool inplace)
{
	return execute(device, "clamp", {tensorA}, {constB, constC}, true, inplace);
}

VulkanTensor min(VulkanDevice device, VulkanTensor tensorA, VulkanTensor tensorB, bool inplace)
{
	return execute(device, "min", {tensorA, tensorB}, {}, false, inplace);
}

VulkanTensor min(VulkanDevice device, VulkanTensor tensorA, float constB, bool inplace)
{
	return execute(device, "min", {tensorA}, {constB}, true, inplace);
}

VulkanTensor max(VulkanDevice device, VulkanTensor tensorA, VulkanTensor tensorB, bool inplace)
{
	return execute(device, "max", {tensorA, tensorB}, {}, false, inplace);
}

VulkanTensor max(VulkanDevice device, VulkanTensor tensorA, float constB, bool inplace)
{
	return execute(device, "max", {tensorA}, {constB}, true, inplace);
}

VulkanTensor add(VulkanDevice device, VulkanTensor tensorA, VulkanTensor tensorB, bool inplace)
{
	return execute(device, "add", {tensorA, tensorB}, {}, false, inplace);
}

VulkanTensor add(VulkanDevice device, VulkanTensor tensorA, float constB, bool inplace)
{
	return execute(device, "add", {tensorA}, {constB}, true, inplace);
}

VulkanTensor sub(VulkanDevice device, VulkanTensor tensorA, VulkanTensor tensorB, bool inplace)
{
	return execute(device, "sub", {tensorA, tensorB}, {}, false, inplace);
}

VulkanTensor sub(VulkanDevice device, VulkanTensor tensorA, float constB, bool inplace)
{
	return execute(device, "sub", {tensorA}, {constB}, true, inplace);
}

VulkanTensor mul(VulkanDevice device, VulkanTensor tensorA, VulkanTensor tensorB, bool inplace)
{
	return execute(device, "mul", {tensorA, tensorB}, {}, false, inplace);
}

VulkanTensor mul(VulkanDevice device, VulkanTensor tensorA, float constB, bool inplace)
{
	return execute(device, "mul", {tensorA}, {constB}, true, inplace);
}

VulkanTensor div(VulkanDevice device, VulkanTensor tensorA, VulkanTensor tensorB, bool inplace)
{
	return execute(device, "div", {tensorA, tensorB}, {}, false, inplace);
}

VulkanTensor div(VulkanDevice device, VulkanTensor tensorA, float constB, bool inplace)
{
	return execute(device, "div", {tensorA}, {constB}, true, inplace);
}

VulkanTensor linear(VulkanDevice device, VulkanTensor tensorA, VulkanTensor tensorB, float alpha, float beta, bool inplace)
{
	return execute(device, "linear", {tensorA, tensorB}, {alpha, beta}, false, inplace);
}

VulkanTensor linear(VulkanDevice device, VulkanTensor tensorA, float alpha, float beta, bool inplace)
{
	return execute(device, "linear", {tensorA}, {alpha, beta, 1.0f}, true, inplace);
}

VulkanTensor eq(VulkanDevice device, VulkanTensor tensorA, VulkanTensor tensorB, bool inplace)
{
	return execute(device, "eq", {tensorA, tensorB}, {}, false, inplace);
}

VulkanTensor eq(VulkanDevice device, VulkanTensor tensorA, float constB, bool inplace)
{
	return execute(device, "eq", {tensorA}, {constB}, true, inplace);
}

VulkanTensor gt(VulkanDevice device, VulkanTensor tensorA, VulkanTensor tensorB, bool inplace)
{
	return execute(device, "gt", {tensorA, tensorB}, {}, false, inplace);
}

VulkanTensor gt(VulkanDevice device, VulkanTensor tensorA, float constB, bool inplace)
{
	return execute(device, "gt", {tensorA}, {constB}, true, inplace);
}

VulkanTensor geq(VulkanDevice device, VulkanTensor tensorA, VulkanTensor tensorB, bool inplace)
{
	return execute(device, "geq", {tensorA, tensorB}, {}, false, inplace);
}

VulkanTensor geq(VulkanDevice device, VulkanTensor tensorA, float constB, bool inplace)
{
	return execute(device, "geq", {tensorA}, {constB}, true, inplace);
}

VulkanTensor lt(VulkanDevice device, VulkanTensor tensorA, VulkanTensor tensorB, bool inplace)
{
	return execute(device, "lt", {tensorA, tensorB}, {}, false, inplace);
}

VulkanTensor lt(VulkanDevice device, VulkanTensor tensorA, float constB, bool inplace)
{
	return execute(device, "lt", {tensorA}, {constB}, true, inplace);
}

VulkanTensor leq(VulkanDevice device, VulkanTensor tensorA, VulkanTensor tensorB, bool inplace)
{
	return execute(device, "leq", {tensorA, tensorB}, {}, false, inplace);
}

VulkanTensor leq(VulkanDevice device, VulkanTensor tensorA, float constB, bool inplace)
{
	return execute(device, "leq", {tensorA}, {constB}, true, inplace);
}

VulkanTensor sin(VulkanDevice device, VulkanTensor tensorA, bool inplace)
{
	return execute(device, "sin", {tensorA}, {}, false, inplace);
}

VulkanTensor cos(VulkanDevice device, VulkanTensor tensorA, bool inplace)
{
	return execute(device, "cos", {tensorA}, {}, false, inplace);
}

VulkanTensor tan(VulkanDevice device, VulkanTensor tensorA, bool inplace)
{
	return execute(device, "tan", {tensorA}, {}, false, inplace);
}

VulkanTensor exp(VulkanDevice device, VulkanTensor tensorA, bool inplace)
{
	return execute(device, "exp", {tensorA}, {}, false, inplace);
}

VulkanTensor log(VulkanDevice device, VulkanTensor tensorA, bool inplace)
{
	return execute(device, "log", {tensorA}, {}, false, inplace);
}

VulkanTensor pow(VulkanDevice device, VulkanTensor tensorA, VulkanTensor tensorB, bool inplace)
{
	return execute(device, "pow", {tensorA, tensorB}, {}, false, inplace);
}

VulkanTensor pow(VulkanDevice device, VulkanTensor tensorA, float constB, bool inplace)
{
	return execute(device, "pow", {tensorA}, {constB}, true, inplace);
}

VulkanTensor range(VulkanDevice device, VulkanTensor tensorA, float start, float step)
{
	return execute(device, "range", {tensorA}, {start, step}, false, false);
}

VulkanTensor fill(VulkanDevice device, VulkanTensor tensorA, float constB)
{
	return execute(device, "fill", {tensorA}, {constB}, false, false);
}

}
