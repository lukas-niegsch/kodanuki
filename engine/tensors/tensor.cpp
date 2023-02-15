#include "engine/tensors/tensor.h"

namespace kodanuki
{

struct TensorState
{
	VulkanDevice device;
};

Tensor::Tensor(TensorBuilder builder)
{
	state = std::make_shared<TensorState>(builder.device);
}

}
