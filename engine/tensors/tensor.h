#pragma once
#include "engine/tensors/variable.h"
#include "engine/vulkan/device.h"
#include <memory>

namespace kodanuki
{

struct TensorBuilder
{
	VulkanDevice device;
	VariableType dtype;
	std::vector<uint32_t> shape;
};

class Tensor
{
public:
	Tensor(TensorBuilder builder);

private:
	std::shared_ptr<struct TensorState> state;
};

}
