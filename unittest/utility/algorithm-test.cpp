#include "engine/utility/algorithm/sorted_intersection.h"
#include <doctest/doctest.h>
#include <set>
#include <iostream>
using namespace Kodanuki;

TEST_CASE("is any match")
{
	std::vector<int> s1 = {2, 3, 1};
	std::vector<int> s2 = {2, 0, 3};
	std::vector<int> s3 = {1, 3, 2, 4};

	CHECK(is_any_match(3, s1, s2) == true);
	CHECK(is_any_match(3, s1, s3) == true);
	CHECK(is_any_match(3, s2, s3) == false);
	CHECK(is_any_match(3, s3, s1) == true);
}

TEST_CASE("sorted multiset intersection")
{
	std::set<int> s1 = {2, 3, 3, 4, 5, 7, 10, 12};
	std::set<int> s2 = {2, 3, 5, 10, 11, 12, 13};
	std::set<int> s3 = {3, 4, 5, 10, 12};

	std::vector<int> result1;
	sorted_intersection(std::back_inserter(result1), s1, s2, s3);
	CHECK(result1 == std::vector<int>({3, 5, 10, 12}));

	std::vector<int> result2;
	sorted_intersection(std::back_inserter(result2), s1, s2);
	CHECK(result2 == std::vector<int>({2, 3, 5, 10, 12}));
}
