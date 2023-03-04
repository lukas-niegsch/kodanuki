#pragma once
#include <vector>
#include <cassert>

namespace kodanuki
{

enum class OperatorType
{
	eLinear,
	eFill,
};

enum class Mutability
{
	eMutable,
	eConstant,
};

template <typename Tensor, typename T>
class Operator
{
public:
	struct OperatorBuilder
	{
		OperatorType type;
		std::vector<T> constants;
		std::vector<Mutability> mutables;
		std::vector<Tensor> tensors;
	};
	Operator(OperatorBuilder builder)
	{ 
		this->builder = builder;
	}

	OperatorType get_type() const
	{
		return builder.type;
	}

	T get_constant(std::size_t index) const
	{
		return builder.constants[index];
	}

	Mutability get_mutable(std::size_t index) const
	{
		return builder.mutables[index];
	}

	Tensor get_tensor(std::size_t index) const
	{
		return builder.tensors[index];
	}

private:
	OperatorBuilder builder;
};

template <typename Tensor>
struct TensorOperator
{
	template <typename T>
	static Tensor fill(Tensor& tensor, const T& value)
	{
		Tensor output(tensor.get_builder());
		Tensor::ifill(output, value);
		return output;
	}

	template <typename T>
	static void ifill(Tensor& tensor, const T& value)
	{
		Operator<Tensor, T> ops = {{
			.type = OperatorType::eFill,
			.constants = {value},
			.mutables = {Mutability::eMutable},
			.tensors = {tensor}
		}};
		Tensor::execute(ops);
	}
};

}
