#pragma once
#include <vector>

namespace kodanuki
{

/**
 * Finds the smallest index of an element below some threshold.
 * Returns the size of the vector if all elements are above it.
 * 
 * @param elements The vector of elements for some totally-ordered type.
 * @param threshold The value with which to compare the elements.
 * @return The smallest index of an element below some threshold.
 */
template <typename T>
std::size_t get_smallest_index_below(const std::vector<T>& elements, const T& threshold)
{
	for (std::size_t i = 0; i < elements.size(); i++) {
		if (elements[i] <= threshold) {
			return i;
		}
	}
	return elements.size();
}

}
