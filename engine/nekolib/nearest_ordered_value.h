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
