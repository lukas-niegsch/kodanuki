#include "engine/utility/signature.h"
#include "engine/utility/type_name.h"
#include "engine/utility/type_union.h"
#include <doctest/doctest.h>
using namespace kodanuki;

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

void example(float a, int b, double c);

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
}

