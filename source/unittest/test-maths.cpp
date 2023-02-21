#include "engine/tensors/tensor.h"
#include "engine/maths/math.h"
#include <doctest/doctest.h>
#include <optional>
using namespace kodanuki;

TensorRuntime create_runtime()
{
	std::optional<TensorRuntime> runtime_wrapper;
	
	SUBCASE("using default CPU runtime")
	{
		runtime_wrapper = TensorRuntime();
	}

	return runtime_wrapper.value();
}

TEST_CASE("test creating zero tensor")
{
	TensorRuntime runtime = create_runtime();

	Tensor a = {{
		.shape = {2, 2},
		.dtype = Memory::FLOAT,
		.runtime = runtime
	}};

	math::make_zeros_inplace(a);
	CHECK(a[{0, 0}].as_float() == doctest::Approx(0));
	CHECK(a[{0, 1}].as_float() == doctest::Approx(0));
	CHECK(a[{1, 0}].as_float() == doctest::Approx(0));
	CHECK(a[{1, 1}].as_float() == doctest::Approx(0));
}

TEST_CASE("test linear functions")
{

}
