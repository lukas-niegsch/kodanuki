#include "engine/utility/vector.h"
#include <doctest/doctest.h>
using namespace Kodanuki;
#include <cmath>

TEST_CASE("transform tests")
{
	Vector2D vector = {0.0, 1.0};
	CHECK(std::abs(vector.x - 0.0) <= 0.001);
	CHECK(std::abs(vector.y - 1.0) <= 0.001);

	Vector3D<int> intVector = {1, 1, 0};
	CHECK(intVector.x == 1);
	CHECK(intVector.y == 1);
	CHECK(intVector.z == 0);

	Vector4D<int> cyan = {0, 255, 255, 255};
	CHECK(cyan.x == 0);
	CHECK(cyan.y == 255);
	CHECK(cyan.z == 255);
	CHECK(cyan.w == 255);
}
