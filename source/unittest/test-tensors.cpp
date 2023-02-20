#include "engine/tensors/memory.h"
#include "engine/tensors/function.h"
#include "engine/tensors/runtime.h"
#include "engine/tensors/scheduler.h"
#include "engine/tensors/tensor.h"
#include "engine/maths/math.h"
#include <doctest/doctest.h>
using namespace kodanuki;


TEST_CASE("Tensor API usage example")
{
	// Create a runtime that is responsible for low-level execution of tasks.
	// There can be different runtimes, such as one for GPU, CPU and such. The
	// default one will execute commands on the CPU, see the constructors.
	TensorRuntime runtime;

	// Using this runtime we can create tensors. Tensors are n-dimensional
	// matrices that own some memory. Here we create a xyz-tensor with 60
	// entries. The tensor values are unspecified, but the memory is allocated. 
	Tensor a = {{
		.shape = {3, 4, 5},
		.dtype = Memory::FLOAT,
		.runtime = runtime
	}};

	// Both loading data into the vector and performing calculations are done
	// with functions. An function can be anything that has input tensors
	// and produces output tensors. Inplace-functions are also possible.
	math::make_zeros_inplace(a);
	a = math::cos(a);
	a = math::add(a, a);
	math::add_inplace(a, a);

	// All classes inside the tensor library handle their own memory and are
	// deleted once no longer used. For this example all tensors are freed
	// once the end of this test block is reached.
}
