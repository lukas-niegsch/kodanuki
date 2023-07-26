#include "engine/nekolib/algorithm/vebsort.h"
#include <doctest/doctest.h>
#include <bits/stdc++.h>
using namespace kodanuki;

TEST_CASE("vebsort")
{
	SUBCASE("works for only for unique values")
	{
		std::vector<uint16_t> values = {
			4, 7, 12, 3, 96, 77, 88, 34, 52
		};
		vebsort<256>(values);
		REQUIRE(values.size() == 9);
		CHECK(values[0] == 3);
		CHECK(values[1] == 4);
		CHECK(values[2] == 7);
		CHECK(values[3] == 12);
		CHECK(values[4] == 34);
		CHECK(values[5] == 52);
		CHECK(values[6] == 77);
		CHECK(values[7] == 88);
		CHECK(values[8] == 96);
	}
}
