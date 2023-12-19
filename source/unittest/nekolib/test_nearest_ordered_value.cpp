#include "engine/nekolib/nearest_ordered_value.h"
#include <doctest/doctest.h>
#include <bits/stdc++.h>
using namespace kodanuki;


TEST_CASE("Nearest Smaller Values Example")
{
	std::vector<int> numbers = { 0, 8, 4, 12, 2, 10, 6, 14, 1, 9, 5, 13};
	std::vector<int> expects = {-1, 0, 0,  4, 0,  2, 2,  6, 0, 1, 1,  5};
	std::vector<int> nearest = prev_ordered_index(numbers, std::less<>{});
	for (std::size_t i = 0; i < numbers.size(); i++) {
		if (nearest[i] == -1) {
			CHECK(expects[i] == -1);
		} else {
			CHECK(numbers[nearest[i]] == expects[i]);
		}
	}
}

TEST_CASE("Reversed Nearest Smaller Values Example")
{
	std::vector<int> numbers = { 0, 8, 4, 12, 2, 10, 6, 14,  1,  9,  5, 13};
	std::vector<int> expects = {-1, 4, 2,  2, 1,  6, 1,  1, -1,  5, -1, -1};
	std::vector<int> nearest = next_ordered_index(numbers, std::less<>{});
	for (std::size_t i = 0; i < numbers.size(); i++) {
		if (nearest[i] == -1) {
			CHECK(expects[i] == -1);
		} else {
			CHECK(numbers[nearest[i]] == expects[i]);
		}
	}
}
