#include "engine/nekolib/templates/type_name.h"
#include <doctest/doctest.h>
#include <bits/stdc++.h>
using namespace kodanuki;


struct Custom {};


TEST_CASE("type_name")
{
	CHECK(type_name<int>() == "int");
	CHECK(type_name<float>() == "float");
	CHECK(type_name<Custom>() == "Custom");
	CHECK(type_name<std::tuple<int, int>>() == "std::tuple<int, int>");
}
