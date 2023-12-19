#pragma once
#include "engine/nekolib/nearest_ordered_value.h"
#include <concepts>
#include <functional>
#include <vector>


namespace kodanuki
{

/**
 * Constructs the cartesian tree for the given numbers.
 *
 * The output vector contains the parent positions of nodes in the
 * cartesian tree such that:
 *     - numbers[out[i]] = "parent of numbers[i]"
 *
 * see also:
 * https://en.wikipedia.org/wiki/Cartesian_tree
 *
 * @param numbers The unique input nodes inside the tree.
 * @return The index of the parent for each node or -1.
 */
template <std::totally_ordered T>
std::vector<int> cartesian_parent_index(const std::vector<T>& numbers)
{
	std::vector<int> nearest_l2r = prev_ordered_index(numbers, std::less<>{});
	std::vector<int> nearest_r2l = next_ordered_index(numbers, std::less<>{});
	std::vector<int> parents(numbers.size());
	for (std::size_t i = 0; i < numbers.size(); i++) {
		int l = nearest_l2r[i];
		int r = nearest_r2l[i];
		parents[i] = numbers[l] >= numbers[r] ? l : r;
	}
	return parents;
}

}

#ifdef KODANUKI_ENABLE_UNITTESTS
#include <doctest/doctest.h>

TEST_CASE("Cartesian Tree Example")
{
	std::vector<int> numbers = {9, 3, 7, 1, 8, 12, 10, 20, 15, 18, 5};
	std::vector<int> parents = {3, 1, 3, 1, 5, 10,  8, 15, 10, 15, 1};
	std::vector<int> tree = kodanuki::cartesian_parent_index(numbers);
	CHECK(parents[ 0] == numbers[tree[ 0]]);
	CHECK(parents[ 1] == numbers[tree[ 1]]);
	CHECK(parents[ 2] == numbers[tree[ 2]]);
	CHECK(tree[3] == -1);
	CHECK(parents[ 4] == numbers[tree[ 4]]);
	CHECK(parents[ 5] == numbers[tree[ 5]]);
	CHECK(parents[ 6] == numbers[tree[ 6]]);
	CHECK(parents[ 7] == numbers[tree[ 7]]);
	CHECK(parents[ 8] == numbers[tree[ 8]]);
	CHECK(parents[ 9] == numbers[tree[ 9]]);
	CHECK(parents[10] == numbers[tree[10]]);
}

#endif
