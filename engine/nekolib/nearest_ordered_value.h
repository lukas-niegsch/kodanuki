#pragma once
#include <algorithm>
#include <concepts>
#include <functional>
#include <vector>


namespace kodanuki
{

/**
 * Finds the previous nearest index in the given order.
 *
 * Example:
 * If the order is std::less, it yields the nearest smaller value.
 * https://en.wikipedia.org/wiki/All_nearest_smaller_values
 *
 * Details:
 * The input is given as the range [begin, end). For each position we
 * yield the index of the previous nearest in-order element. If there
 * is no in-order element we yield -1. The time and space complexity
 * is O(n) where n is the number of elements inside the range.
 *
 * @param numbers The numbers in the range that are unique.
 * @param ordered The comparison operation for the order.
 * @return An index vector into the numbers vectors.
 */
template <std::totally_ordered T, class Compare = std::less<T>>
std::vector<int> prev_ordered_index(const std::vector<T>& numbers, Compare ordered = Compare{})
{
	int n = numbers.size();
	std::vector<int> nearest_indices(n, -1);
	for (int current = 1; current < n; current++) {
		int previous = current - 1;
		while (!(previous == -1 || ordered(numbers[previous], numbers[current]))) {
			previous = nearest_indices[previous];
		}
		nearest_indices[current] = previous;
	}
	return nearest_indices;
}

/**
 * Finds the next nearest index in the given order.
 *
 * Example:
 * If the order is std::less, it yields the reversed nearest smaller value.
 * https://en.wikipedia.org/wiki/All_nearest_smaller_values
 *
 * Details:
 * The input is given as the range [begin, end). For each position we
 * yield the index of the next nearest in-order element. If there
 * is no in-order element we yield -1. The time and space complexity
 * is O(n) where n is the number of elements inside the range.
 *
 * @param numbers The numbers in the range that are unique.
 * @param ordered The comparison operation for the order.
 * @return An index vector into the numbers vectors.
 */
template <std::totally_ordered T, class Compare = std::less<T>>
std::vector<int> next_ordered_index(const std::vector<T>& numbers, Compare ordered = Compare{})
{
	std::vector<T> reversed_numbers(numbers.rbegin(), numbers.rend());
	std::vector<int> nearest_indices = prev_ordered_index(reversed_numbers, ordered);
	auto mirror = [&](int& x) { if (x != -1) x = numbers.size() - x - 1; };
	std::ranges::for_each(nearest_indices, mirror);
	std::ranges::reverse(nearest_indices);
	return nearest_indices;
}

}

#ifdef KODANUKI_ENABLE_UNITTESTS
#include <doctest/doctest.h>

TEST_CASE("Nearest Smaller Values Example")
{
	std::vector<int> numbers = { 0, 8, 4, 12, 2, 10, 6, 14, 1, 9, 5, 13};
	std::vector<int> expects = {-1, 0, 0,  4, 0,  2, 2,  6, 0, 1, 1,  5};
	std::vector<int> nearest = kodanuki::prev_ordered_index(numbers, std::less<>{});
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
	std::vector<int> nearest = kodanuki::next_ordered_index(numbers, std::less<>{});
	for (std::size_t i = 0; i < numbers.size(); i++) {
		if (nearest[i] == -1) {
			CHECK(expects[i] == -1);
		} else {
			CHECK(numbers[nearest[i]] == expects[i]);
		}
	}
}

#endif
