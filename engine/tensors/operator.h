#pragma once
#include "engine/tensors/memory.h"

namespace kodanuki
{

class Operator
{
public:
	virtual ~Operator();

public:
	virtual void apply() = 0;
};

}
