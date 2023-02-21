#include "engine/tensors/tensor.h"

namespace kodanuki
{

struct TensorState
{
	Memory memory;
	std::vector<std::size_t> shape;
	Memory::DataType dtype;
	TensorRuntime runtime;
};

Tensor::Tensor(TensorBuilder builder)
{
	state = std::make_shared<TensorState>();
	state->shape = builder.shape;
	state->dtype = builder.dtype;
	state->runtime = builder.runtime;
}

std::vector<std::size_t> Tensor::get_shape() const
{
	return state->shape;
}

Memory::DataType Tensor::get_dtype() const
{
	return state->dtype;
}

TensorRuntime Tensor::get_runtime() const
{
	return state->runtime;
}

MemoryView Tensor::operator[] (std::vector<std::size_t> index)
{
	(void) index;
	return state->memory.make_view();
}

}
