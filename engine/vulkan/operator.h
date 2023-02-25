#pragma once
#include <vector>
#include <cassert>

namespace kodanuki
{

enum class OperatorType
{
	eFill,
	eLinear,
	eCos,
	eSin,
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
	Operator(OperatorBuilder builder) { (void) builder; }
};

template <typename Tensor>
struct TensorOperator
{
	static Tensor add(Tensor& tensorA, Tensor& tensorB)
	{
		assert(tensorA.get_shape() == tensorB.get_shape());
		return tensorA; // TODO: implement method
	}

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

	static void icos(Tensor& tensor)
	{
		(void) tensor;
		// TODO: implement method
	}
};

}
