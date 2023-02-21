#pragma once
#include "engine/tensors/operator.h"

namespace kodanuki
{

class TensorRuntime
{
public:
	void register_operator(const Operator& op);
};

}
