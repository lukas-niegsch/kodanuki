/*******************************************************************************
 *                  ____  ____  ____  ____  _  _  ____  ____
 *                 / ___)(_  _)(  _ \(  __)( \/ )(  _ \(  _ \
 *                 \___ \  )(   )   / ) _)  )  (  ) __/ )   /
 *                 (____/ (__) (__\_)(____)(_/\_)(__)  (__\_)
 *
 * A simple library for processing strings and string expressions.
 *
 * MIT License <https://opensource.org/licenses/MIT>
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2023 Lukas Niegsch
 ******************************************************************************/
#pragma once
#include <doctest/doctest.h>
#include <bits/stdc++.h>


namespace kodanuki::strexpr
{

/**
 * Finds the nearest index in the given order.
 *
 * Example:
 * If the comparison is std::less, it yields the nearest smaller value.
 * https://en.wikipedia.org/wiki/All_nearest_smaller_values
 *
 * Details:
 * The input is given as the range [begin, end). For each position we
 * yield the index of the nearest previous in-order element. If there
 * is no in-order element we yield -1. The input iterators are either
 * normal or reverse iterators depending on searching from left-to-right
 * or from right-to-left. The output will be in the same direction as
 * the iterator order. The time and space complexity is O(n) where n
 * is the number of elements inside the range.
 *
 * @param numbers The start of the element range (inclusive).
 * @param sentinel The end of the element range (exclusive).
 * @param ordered The comparison operation for the order.
 */
template <std::totally_ordered T, std::random_access_iterator I, class Compare = std::less<T>>
requires  std::same_as<T, typename std::iterator_traits<I>::value_type>
std::vector<int> nearest_ordered_indices(I numbers, I sentinel, Compare ordered = Compare{})
{
	int n = std::distance(numbers, sentinel);
	std::vector<int> nearest_indices(n, -1);
	for (int current = 1; current < n; current++) {
		int previous = current - 1;
		while (!(previous == -1 || ordered(numbers[previous], numbers[current]))) {
			previous = nearest_indices[previous];
		}
		nearest_indices[current] = previous;
	}
	bool is_reverse = std::distance(sentinel.base(), numbers.base()) > 0;
	if (is_reverse) {
		auto mirror = [&](int& x) { if (x != -1) x = n - x - 1; };
		std::ranges::for_each(nearest_indices, mirror);
		std::ranges::reverse(nearest_indices);
	}
	return nearest_indices;
}

TEST_CASE("Nearest Smaller Values Wikipedia Example")
{
	std::vector<int> numbers = { 0, 8, 4, 12, 2, 10, 6, 14, 1, 9, 5, 13, 3, 11, 7, 15};
	std::vector<int> expects = {-1, 0, 0,  4, 0,  2, 2,  6, 0, 1, 1,  5, 1,  3, 3,  7};
	std::vector<int> nearest = nearest_ordered_indices<int>(numbers.begin(), numbers.end());
	for (std::size_t i = 0; i < numbers.size(); i++) {
		if (nearest[i] == -1) {
			CHECK(expects[i] == -1);
		} else {
			CHECK(numbers[nearest[i]] == expects[i]);
		}
	}
}

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
std::vector<int> cartesian_parent_indices(const std::vector<T>& numbers)
{
	std::vector<int> left_nearest = nearest_ordered_indices<T>(numbers.begin(), numbers.end());
	std::vector<int> right_nearest = nearest_ordered_indices<T>(numbers.rbegin(), numbers.rend());
	std::vector<int> parents(numbers.size());
	for (std::size_t i = 0; i < numbers.size(); i++) {
		int l = left_nearest[i];
		int r = right_nearest[i];
		parents[i] = numbers[l] >= numbers[r] ? l : r;
	}
	return parents;
}

TEST_CASE("Cartesian Tree Wikipedia Example")
{
	std::vector<int> numbers = {9, 3, 7, 1, 8, 12, 10, 20, 15, 18, 5};
	std::vector<int> parents = {3, 1, 3, 1, 5, 10,  8, 15, 10, 15, 1};
	std::vector<int> tree = cartesian_parent_indices(numbers);
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

}
