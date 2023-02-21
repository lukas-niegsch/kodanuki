#include "engine/tensors/tensor.h"

namespace kodanuki
{

Tensor::Tensor(TensorBuilder builder)
{
	(void) builder;
}

MemoryView Tensor::operator[] (std::vector<std::size_t> index)
{
	(void) index;
	return {};
}

}
