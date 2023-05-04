#include <algorithm>
#include <vector>

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

}
