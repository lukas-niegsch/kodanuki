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
