#pragma once
#include <vector>
#include <cassert>

namespace kodanuki
{

enum class OperatorType
{
	eLinear,
	eFill,
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
		assert(tensorA.get_dtype() == tensorB.get_dtype());
		Tensor output(tensorA.get_builder());
		Operator<Tensor, float> ops = {{
			.type = OperatorType::eLinear,
			.constants = {1.0f, 1.0f},
			.mutables = {Mutability::eConstant, Mutability::eConstant, Mutability::eMutable},
			.tensors = {tensorA, tensorB, output}
		}};
		Tensor::execute(ops);
		return output;
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
