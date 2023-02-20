#pragma once
#include "engine/tensors/function.h"
#include "engine/tensors/tensor.h"

namespace kodanuki::math
{

void make_zeros_inplace(Tensor& a);
void add_inplace(Tensor& a, const Tensor& b);
Tensor cos(const Tensor& a);
Tensor add(const Tensor& a, const Tensor& b);
Tensor copy(const Tensor& a);

}
