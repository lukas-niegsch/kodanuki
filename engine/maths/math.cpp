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
	tensor[{0}].as_float() = value;
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
