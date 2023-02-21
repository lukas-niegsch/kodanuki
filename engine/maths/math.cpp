#include "engine/maths/math.h"

namespace kodanuki::op
{

Tensor fill(const Tensor& tensor, float value)
{
	Tensor output = {{
		.shape = tensor.get_shape(),
		.dtype = tensor.get_dtype(),
		.runtime = tensor.get_runtime()
	}};
	op::ifill(output, value);
	return output;
}

void ifill(Tensor& tensor, float value)
{
	Memory memory = tensor.get_memory();
	for (std::size_t i = 0; i < memory.get_size(); i++) {
		MemoryView view = memory.make_view(i);
		view.as_float() = value;
	}
}

Tensor fill(const Tensor& tensor, int value)
{
	(void) tensor;
	(void) value;
	return tensor;
}

void ifill(Tensor& tensor, int value)
{
	(void) tensor;
	(void) value;
}

Tensor diag(const Tensor& tensor, float value)
{
	Tensor output = op::copy(tensor);
	std::size_t dimensions = output.get_shape().size();

	for (std::size_t i = 0; i < dimensions; i++)
	{
		std::vector<std::size_t> index(dimensions, i);
		output[index].as_float() = value;
	}
	
	return output;
}

void idiag(Tensor& tensor, float value)
{
	(void) tensor;
	(void) value;
}

Tensor cos(const Tensor& a)
{
	(void) a;
	return a;
}

void icos(const Tensor& a)
{
	(void) a;
}

Tensor add(const Tensor& a, const Tensor& b)
{
	(void) a;
	(void) b;
	return a;
}

void iadd(Tensor& a, const Tensor& b)
{
	(void) a;
	(void) b;
}

Tensor copy(const Tensor& a)
{
	(void) a;
	return a;
}

void icopy(Tensor& a)
{
	(void) a;
}

}
