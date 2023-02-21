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

TEST_CASE("test creating constant tensor")
{
	TensorRuntime runtime = create_runtime();
	
	Tensor a = {{
		.shape = {2, 2},
		.dtype = Memory::FLOAT,
		.runtime = runtime
	}};

	op::ifill(a, 0);
	CHECK(a[{0, 0}].as_float() == doctest::Approx(0));
	CHECK(a[{0, 1}].as_float() == doctest::Approx(0));
	CHECK(a[{1, 0}].as_float() == doctest::Approx(0));
	CHECK(a[{1, 1}].as_float() == doctest::Approx(0));

	op::ifill(a, 3.14f);
	CHECK(a[{0, 0}].as_float() == doctest::Approx(3.14f));
	CHECK(a[{0, 1}].as_float() == doctest::Approx(3.14f));
	CHECK(a[{1, 0}].as_float() == doctest::Approx(3.14f));
	CHECK(a[{1, 1}].as_float() == doctest::Approx(3.14f));

	op::ifill(a, 2.0f);
	CHECK(a[{0, 0}].as_float() == doctest::Approx(2.0f));
	CHECK(a[{0, 1}].as_float() == doctest::Approx(2.0f));
	CHECK(a[{1, 0}].as_float() == doctest::Approx(2.0f));
	CHECK(a[{1, 1}].as_float() == doctest::Approx(2.0f));

	Tensor b = op::fill(a, 0);
	CHECK(b[{0, 0}].as_float() == doctest::Approx(0));
	CHECK(b[{0, 1}].as_float() == doctest::Approx(0));
	CHECK(b[{1, 0}].as_float() == doctest::Approx(0));
	CHECK(b[{1, 1}].as_float() == doctest::Approx(0));

	Tensor c = op::fill(a, 3.14f);
	CHECK(c[{0, 0}].as_float() == doctest::Approx(3.14f));
	CHECK(c[{0, 1}].as_float() == doctest::Approx(3.14f));
	CHECK(c[{1, 0}].as_float() == doctest::Approx(3.14f));
	CHECK(c[{1, 1}].as_float() == doctest::Approx(3.14f));

	Tensor d = op::fill(a, 2.0f);
	CHECK(d[{0, 0}].as_float() == doctest::Approx(2.0f));
	CHECK(d[{0, 1}].as_float() == doctest::Approx(2.0f));
	CHECK(d[{1, 0}].as_float() == doctest::Approx(2.0f));
	CHECK(d[{1, 1}].as_float() == doctest::Approx(2.0f));
}

TEST_CASE("test linear functions")
{

}
