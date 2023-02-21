#include "engine/tensors/tensor.h"
#include <iostream>

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

	std::size_t size = 1;
	for (std::size_t n : builder.shape) {
		size *= n;
	}
	state->memory.resize(size);
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

Memory Tensor::get_memory()
{
	return state->memory;
}

MemoryView Tensor::operator[] (std::vector<std::size_t> index)
{
	std::size_t single_index = 0;
	std::size_t stride = 1;
	for (std::size_t i = 0; i < index.size(); i++) {
		single_index += index[i] * stride;
		stride *= state->shape[i]; 
	}
	return state->memory.make_view(single_index);
}

}
