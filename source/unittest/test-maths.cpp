#include "engine/maths/indices.h"
#include "engine/maths/maxseq.h"
#include <doctest/doctest.h>
using namespace kodanuki;

TEST_CASE("test get_smallest_index_below algorithm")
{
	std::vector<int> empty_array;
	CHECK(get_smallest_index_below(empty_array, 4) == 0);

	std::vector<int> single_array_above = {5};
	CHECK(get_smallest_index_below(single_array_above, 4) == 1);

	std::vector<int> single_array_equal = {4};
	CHECK(get_smallest_index_below(single_array_equal, 4) == 0);

	std::vector<int> single_array_below = {3};
	CHECK(get_smallest_index_below(single_array_below, 4) == 0);

	std::vector<int> all_greater_array = {1, 2, 3, 4, 5};
	CHECK(get_smallest_index_below(all_greater_array, 0) == all_greater_array.size());

	std::vector<int> mixed_array = {3, 5, -1, -5, 4};
	CHECK(get_smallest_index_below(mixed_array, -3) == 3);
}

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

TEST_CASE("test young_maxseq algorithm")
{
	std::vector<int> empty_array;
	CHECK(young_maxseq(empty_array) == 0);

	std::vector<int> single_array = {42};
	CHECK(young_maxseq(single_array) == 1);

	std::vector<int> two_sorted_array = {42, 43};
	CHECK(young_maxseq(two_sorted_array) == 2);

	std::vector<int> two_reversed_array = {43, 42};
	CHECK(young_maxseq(two_reversed_array) == 1);

	std::vector<int> long_increasing_array = {1, 2, 3, 4, 5};
	CHECK(young_maxseq(long_increasing_array) == long_increasing_array.size());

	std::vector<int> long_decreasing_array = {5, 4, 3, 2, 1};
	CHECK(young_maxseq(long_decreasing_array) == 1);

	std::vector<int> mixed_array = {1, 3, -1, 4, -3, 5};
	CHECK(young_maxseq(mixed_array) == 4);
}
