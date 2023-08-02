#include "engine/central/family.h"
#include <doctest/doctest.h>
using namespace kodanuki;

TEST_CASE("family tests")
{
	Entity invalid;

	SUBCASE("isolation")
	{
		Entity entity = ECS::create();
		Family& family = ECS::get<Family>(entity);
		CHECK(family.get_root() == entity);
		CHECK(family.get_parent() == invalid);
		CHECK(family.get_siblings().size() == 0);
		CHECK(family.get_children().size() == 0);
	}

	SUBCASE("children")
	{
		Entity parent = ECS::create();
		Entity entity = ECS::create(parent);
		Entity childA = ECS::create(entity);
		Entity childB = ECS::create(entity);

		Family& parentFamily = ECS::get<Family>(parent);
		CHECK(parentFamily.get_root() == parent);
		CHECK(parentFamily.get_parent() == invalid);
		CHECK(parentFamily.get_siblings().size() == 0);
		CHECK(parentFamily.get_children().size() == 1);

		Family& entityFamily = ECS::get<Family>(entity);
		CHECK(entityFamily.get_root() == parent);
		CHECK(entityFamily.get_parent() == parent);
		CHECK(entityFamily.get_siblings().size() == 0);
		CHECK(entityFamily.get_children().size() == 2);
	
		Family& childFamilyA = ECS::get<Family>(childA);
		CHECK(childFamilyA.get_root() == parent);
		CHECK(childFamilyA.get_parent() == entity);
		CHECK(childFamilyA.get_siblings().size() == 1);
		CHECK(childFamilyA.get_children().size() == 0);

		Family& childFamilyB = ECS::get<Family>(childB);
		CHECK(childFamilyB.get_root() == parent);
		CHECK(childFamilyB.get_parent() == entity);
		CHECK(childFamilyB.get_siblings().size() == 1);
		CHECK(childFamilyB.get_children().size() == 0);
	}
};
