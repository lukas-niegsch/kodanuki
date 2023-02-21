#pragma once
#include "engine/tensors/memory.h"
#include "engine/tensors/runtime.h"
#include <vector>
#include <memory>

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
	std::vector<std::size_t> get_shape() const;
	Memory::DataType get_dtype() const;
	TensorRuntime get_runtime() const;
	Memory get_memory();

public:
	bool operator==(const Tensor& other);
	MemoryView operator[] (std::vector<std::size_t> index);

private:
	std::shared_ptr<struct TensorState> state;
};

}
