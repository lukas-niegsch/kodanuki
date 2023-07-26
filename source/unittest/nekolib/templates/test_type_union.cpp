#include "engine/nekolib/templates/type_union.h"
#include <doctest/doctest.h>
#include <bits/stdc++.h>
using namespace kodanuki;


TEST_CASE("type_union")
{
	CHECK(std::is_same_v<
		std::tuple<int>,
		type_union_t<int>
	>);

	CHECK(std::is_same_v<
		std::tuple<int>,
		type_union_t<std::tuple<int>, std::tuple<int>>
	>);

	// All values must be given as tuples!
	CHECK(std::is_same_v<
		std::tuple<int>,
		type_union_t<int, std::tuple<int>>
	> == false);

	CHECK(std::is_same_v<
		std::tuple<int, double, float>,
		type_union_t<std::tuple<int, double>, std::tuple<int, float>>
	>);
}
