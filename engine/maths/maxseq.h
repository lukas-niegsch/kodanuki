#pragma once
#include "engine/maths/indices.h"
#include <algorithm>
#include <limits>
#include <vector>

#include <iostream>

namespace kodanuki
{

/**
 * Computes the maximum subarray sum using Kadane's algorithm.
 *
 * @param elements The vector of elements of some totally-ordered type.
 * @return The largest sum of all subarrays.
 */
template <typename T>
T kadane_maxsum(std::vector<T> elements)
{
	T sum_max = 0;
	T sum_tmp = 0;
	for (auto& element : elements) {
		sum_tmp = std::max(sum_tmp + element, 0);
		sum_max = std::max(sum_tmp, sum_max);
	}
	return sum_max;
}

/**
 * Computes the length of the longest increasing subsequence using Young's
 * tableau algorithm.
 *
 * @param elements The vector of elements of some totally-ordered type.
 * @return The length of the largest increasing subsequence.
 */
template <typename T>
int young_maxseq(std::vector<T> elements)
{
	std::vector<T> endings;
	for (auto& element : elements) {
		auto it = std::lower_bound(endings.begin(), endings.end(), element);
		if (it == endings.end()) {
			endings.push_back(element);
		} else {
			*it = element;
		}
	}
	std::size_t index = get_smallest_index_below(endings, std::numeric_limits<T>::min());
	return std::max(0, static_cast<int>(index));
}

}
