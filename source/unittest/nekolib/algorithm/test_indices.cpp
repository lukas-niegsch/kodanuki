#include "engine/nekolib/algorithm/indices.h"
#include <doctest/doctest.h>
#include <bits/stdc++.h>
using namespace kodanuki;

TEST_CASE("get_smallest_index_below")
{
	std::vector<int> items = {
		74, 63, 42, 42, 42, 29, 29, 17, 12, 42
	};

	SUBCASE("empty vector returns zero")
	{
		items.clear();
		CHECK(get_smallest_index_below(items, 20) == 0);
		CHECK(get_smallest_index_below(items, 30) == 0);
	}

	SUBCASE("finds the smallest index below")
	{
		CHECK(get_smallest_index_below(items, 13) == 8);
		CHECK(get_smallest_index_below(items, 65) == 1);
	}

	SUBCASE("returns the size when no index found")
	{
		REQUIRE(std::ranges::min(items) > 10);
		CHECK(get_smallest_index_below(items, 10) == items.size());
	}

	SUBCASE("equality is not exclusive")
	{
		CHECK(get_smallest_index_below(items, 17) == 7);
		CHECK(get_smallest_index_below(items, 63) == 1);
	}

	SUBCASE("returns the first index for equal items")
	{
		CHECK(get_smallest_index_below(items, 42) == 2);
		CHECK(get_smallest_index_below(items, 29) == 5);
	}
}
