#include "engine/maths/maxsum.h"
#include <doctest/doctest.h>
using namespace kodanuki;

TEST_CASE("test kadane_maxsum algorithm")
{
	std::vector<int> empty_array;
	CHECK(kadane_maxsum(empty_array) == 0);

	std::vector<int> single_positive_array = {42};
	CHECK(kadane_maxsum(single_positive_array) == 42);

	std::vector<int> single_negative_array = {-13};
	CHECK(kadane_maxsum(single_negative_array) == 0);

	std::vector<int> all_zero_array = {0, 0, 0};
	CHECK(kadane_maxsum(all_zero_array) == 0);

	std::vector<int> all_positive_array = {1, 2, 3, 4, 5};
	CHECK(kadane_maxsum(all_positive_array) == 15);

	std::vector<int> all_negative_array = {-1, -2, -3, -1, -4};
	CHECK(kadane_maxsum(all_negative_array) == 0);

	std::vector<int> first_positive_array = {4, -1, -2, -5};
	CHECK(kadane_maxsum(first_positive_array) == 4);

	std::vector<int> last_positive_array = {-1, -2, -1, 4};
	CHECK(kadane_maxsum(last_positive_array) == 4);

	std::vector<int> mixed_array = {-2, 1, -3, 4, -1, 2, 1, -5, 4};
	CHECK(kadane_maxsum(mixed_array) == 6);
}
