#pragma once
#include "engine/tensors/operator.h"
#include "engine/tensors/tensor.h"

namespace kodanuki::op
{

Tensor fill(const Tensor& tensor, float value);
void ifill(Tensor& tensor, float value);

Tensor fill(const Tensor& tensor, int value);
void ifill(Tensor& tensor, int value);

Tensor diag(const Tensor& tensor, float value);
void idiag(Tensor& tensor, float value);

Tensor cos(const Tensor& a);
void icos(const Tensor& a);

Tensor add(const Tensor& a, const Tensor& b);
void iadd(Tensor& a, const Tensor& b);

Tensor copy(const Tensor& a);
void icopy(Tensor& a);

}
