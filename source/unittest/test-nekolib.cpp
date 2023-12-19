#include "engine/central/entity.h"
#include "engine/nekolib/algorithm/indices.h"
#include "engine/nekolib/algorithm/maxseq.h"
#include "engine/nekolib/algorithm/sorted_intersection.h"
#include "engine/nekolib/templates/type_name.h"
#include "engine/nekolib/templates/type_union.h"
#include "engine/nekolib/templates/strong_typedef.h"
#include "engine/nekolib/templates/signature.h"
#include <doctest/doctest.h>
#include <bits/stdc++.h>
using namespace kodanuki;


TEST_CASE("get_smallest_index_below")
{
	std::vector<int> items = {
		74, 63, 42, 42, 42, 29, 29, 17, 12, 42
	};

	SUBCASE("empty vector returns zero")
	{
		items.clear();
		CHECK(get_smallest_index_below(items, 20) == 0);
		CHECK(get_smallest_index_below(items, 30) == 0);
	}

	SUBCASE("finds the smallest index below")
	{
		CHECK(get_smallest_index_below(items, 13) == 8);
		CHECK(get_smallest_index_below(items, 65) == 1);
	}

	SUBCASE("returns the size when no index found")
	{
		REQUIRE(std::ranges::min(items) > 10);
		CHECK(get_smallest_index_below(items, 10) == items.size());
	}

	SUBCASE("equality is not exclusive")
	{
		CHECK(get_smallest_index_below(items, 17) == 7);
		CHECK(get_smallest_index_below(items, 63) == 1);
	}

	SUBCASE("returns the first index for equal items")
	{
		CHECK(get_smallest_index_below(items, 42) == 2);
		CHECK(get_smallest_index_below(items, 29) == 5);
	}
}

TEST_CASE("kadane_maxsum")
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

TEST_CASE("young_maxseq")
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

TEST_CASE("sorted_intersection")
{
	SUBCASE("single set")
	{
		std::set<int> s1 = {2, 3, 4};
		std::vector<int> result;
		sorted_intersection(std::back_inserter(result), s1);
		CHECK(result == std::vector<int>({2, 3, 4}));
	}

	SUBCASE("multiple sets")
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
}

void example(float a, int b, double c);
void empty_example();

struct ExampleStruct
{
	static int static_example(int, int);
	int non_static_example(float);
};

TEST_CASE("signature")
{
	CHECK(std::is_same_v<
		float,
		forward_signature_t<0, example>
	>);

	CHECK(std::is_same_v<
		int,
		forward_signature_t<1, example>
	>);

	CHECK(std::is_same_v<
		double,
		forward_signature_t<2, example>
	>);

	CHECK(std::is_same_v<
		double,
		reverse_signature_t<0, example>
	>);

	CHECK(std::is_same_v<
		int,
		reverse_signature_t<1, example>
	>);

	CHECK(std::is_same_v<
		float,
		reverse_signature_t<2, example>
	>);

	CHECK(std::is_same_v<
		void,
		return_signature_t<example>
	>);

	CHECK(std::is_same_v<
		std::tuple<float, int, double>,
		params_signature_t<example>
	>);

	CHECK(std::is_same_v<
		std::tuple<>,
		params_signature_t<empty_example>
	>);

	CHECK(std::is_same_v<
		int,
		return_signature_t<ExampleStruct::static_example>
	>);

	// Does not work yet!
	// CHECK(std::is_same_v<
	// 	int,
	// 	return_signature_t<ExampleStruct::non_static_example>
	// >);
}

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

// Implementation not correct ... just used for the interface!
class ExampleBuilder
{
public:
	struct Builder { int value; };
	static ExampleBuilder create(const Builder builder) { return ExampleBuilder(builder); }
	ExampleBuilder(const Builder) {}
	~ExampleBuilder() = default;
	ExampleBuilder(const ExampleBuilder&) {}
	ExampleBuilder& operator=(const ExampleBuilder&) { return *this; }
	ExampleBuilder(ExampleBuilder&& ) noexcept {}
	ExampleBuilder& operator=(ExampleBuilder&&) { return *this; }
	bool operator<(const ExampleBuilder&) const { return true; }
	bool operator==(const ExampleBuilder&) const { return true; }

private:
	int data;
};

KODANUKI_STRONG_TYPEDEF(ExampleBuilder, ExampleLike);

TEST_CASE("strong_typedef")
{
	static_assert(buildable<ExampleBuilder>);
	static_assert(buildable<ExampleLike>);
    static_assert(std::move_constructible<ExampleBuilder>);
    static_assert(std::move_constructible<ExampleLike>);
    static_assert(std::copy_constructible<ExampleBuilder>);
    static_assert(std::copy_constructible<ExampleLike>);
    static_assert(buildable<ExampleBuilder>);
    static_assert(buildable<ExampleLike>);

    ExampleBuilder original = ExampleBuilder::create({.value = 10});
    ExampleLike copy = original;
    original = copy;
    copy = std::move(original);
    original = std::move(copy);
    copy = ExampleLike::create({.value = 5});
    CHECK(original == copy); // Only syntax!
    CHECK(original < copy);

    Entity entity = ECS::create();
    ECS::update(entity, ExampleLike::create({.value = 5}));
    CHECK(ECS::has<ExampleBuilder>(entity) == false);
    ECS::remove<ExampleLike>(entity);
    ECS::update(entity, ExampleBuilder::create({.value = 5}));
    CHECK(ECS::has<ExampleLike>(entity) == false);
}

struct Custom {};

TEST_CASE("type_name")
{
	CHECK(type_name<int>() == "int");
	CHECK(type_name<float>() == "float");
	CHECK(type_name<Custom>() == "Custom");
	CHECK(type_name<std::tuple<int, int>>() == "std::tuple<int, int>");
}

TEST_CASE("type_union")
{
	CHECK(std::is_same_v<
		std::tuple<int>,
		type_union_t<int>
	>);

	CHECK(std::is_same_v<
		std::tuple<int>,
		type_union_t<std::tuple<int>, std::tuple<int>>
	>);

	// All values must be given as tuples!
	CHECK(std::is_same_v<
		std::tuple<int>,
		type_union_t<int, std::tuple<int>>
	> == false);

	CHECK(std::is_same_v<
		std::tuple<int, double, float>,
		type_union_t<std::tuple<int, double>, std::tuple<int, float>>
	>);
}
