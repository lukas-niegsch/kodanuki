#include "engine/nekolib/cartesian_tree.h"
#include <doctest/doctest.h>
#include <bits/stdc++.h>
using namespace kodanuki;


TEST_CASE("Cartesian Tree Example")
{
	std::vector<int> numbers = {9, 3, 7, 1, 8, 12, 10, 20, 15, 18, 5};
	std::vector<int> parents = {3, 1, 3, 1, 5, 10,  8, 15, 10, 15, 1};
	std::vector<int> tree = cartesian_parent_index(numbers);
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
