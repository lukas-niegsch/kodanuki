#include "engine/nekolib/templates/stringify.h"
#include <doctest/doctest.h>
#include <bits/stdc++.h>
using namespace kodanuki;

struct Example
{
	int value;
};

std::ostream& operator<<(std::ostream& os, const Example& example)
{
	os << "Example(value = " << example.value << ")";
	return os;
}

struct Another
{
	int value;
};

template <>
std::string kodanuki::stringify(Another info)
{
	return std::string("Another(value = ") + std::to_string(info.value) + ")";
};

TEST_CASE("stringify")
{
	SUBCASE("works with primatives")
	{
		CHECK(stringify(5) == "5");
		CHECK(stringify(5.3f) == "5.3");
		CHECK(stringify(3.14) == "3.14");
		CHECK(stringify("hey") == "hey");
	}

	SUBCASE("works with printables")
	{
		Example example = {42};
		CHECK(stringify(example) == "Example(value = 42)");
	}

	SUBCASE("works with most iterables if underlying types can be stringified")
	{
		std::vector<int> integers = {3, 4, 5};
		CHECK(stringify(integers) == "std::vector<int> {3, 4, 5}");

		std::set<int> integer_set(integers.begin(), integers.end());
		CHECK(stringify(integer_set) == "std::set<int> {3, 4, 5}");
	}

	SUBCASE("element order depends on the datastructure")
	{
		std::set<int> integers = {3, 4, 5, 2, 3, 2};
		CHECK(stringify(integers) == "std::set<int> {2, 3, 4, 5}");
	}

	/**
	 * Okay, this is actually possible to implement. Just pass the type
	 * as reference to the method. However, I don't want that for other
	 * types, because it makes the method harder to use.
	 */
	SUBCASE("does not work with arrays")
	{
		int integers[] = {3, 4, 5};
		CHECK(stringify(integers) != "int [3] {3, 4, 5}");
		CHECK(stringify(integers).starts_with("0x"));
	}

	/**
	 * Overwriting operator<< is better, as it would work automatically
	 * with stringify method but can also be used in other places.
	 */
	SUBCASE("works when specializing template")
	{
		Another example = {42};
		CHECK(stringify(example) == "Another(value = 42)");
	}
}
