#pragma once
#include "engine/tensors/memory.h"
#include "engine/tensors/runtime.h"
#include <vector>

namespace kodanuki
{

class Tensor
{
public:
	struct TensorBuilder
	{
		std::vector<std::size_t> shape;
		Memory::DataType dtype;
		TensorRuntime runtime;
	};
	Tensor(TensorBuilder builder);

public:
	bool operator==(const Tensor& other);

	MemoryView operator[] (std::vector<std::size_t> index);
};

}
