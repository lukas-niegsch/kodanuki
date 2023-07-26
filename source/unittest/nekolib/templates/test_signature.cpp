#include "engine/nekolib/templates/signature.h"
#include <doctest/doctest.h>
#include <bits/stdc++.h>
using namespace kodanuki;

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
