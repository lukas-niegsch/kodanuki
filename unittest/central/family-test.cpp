#include "engine/central/family.h"
#include <doctest/doctest.h>
using namespace Kodanuki;

TEST_CASE("family tests")
{
	Entity invalid;

	SUBCASE("isolation")
	{
		Entity entity = ECS->create();
		update_family(entity);

		Family& family = ECS->get<Family>(entity);
		CHECK(family.root == entity);
		CHECK(family.parent == invalid);
		CHECK(family.itself == entity);
		CHECK(family.siblings.size() == 0);
		CHECK(family.children.size() == 0);
	}

	SUBCASE("children")
	{
		Entity parent = ECS->create();
		Entity entity = ECS->create();
		Entity childA = ECS->create();
		Entity childB = ECS->create();
		update_family(parent);
		update_family(entity, parent);
		update_family(childA, entity);
		update_family(childB, entity);

		Family& parentFamily = ECS->get<Family>(parent);
		CHECK(parentFamily.root == parent);
		CHECK(parentFamily.parent == invalid);
		CHECK(parentFamily.itself == parent);
		CHECK(parentFamily.siblings.size() == 0);
		CHECK(parentFamily.children.size() == 1);

		Family& entityFamily = ECS->get<Family>(entity);
		CHECK(entityFamily.root == parent);
		CHECK(entityFamily.parent == parent);
		CHECK(entityFamily.itself == entity);
		CHECK(entityFamily.siblings.size() == 0);
		CHECK(entityFamily.children.size() == 2);
	
		Family& childFamilyA = ECS->get<Family>(childA);
		CHECK(childFamilyA.root == parent);
		CHECK(childFamilyA.parent == entity);
		CHECK(childFamilyA.itself == childA);
		CHECK(childFamilyA.siblings.size() == 1);
		CHECK(childFamilyA.children.size() == 0);

		Family& childFamilyB = ECS->get<Family>(childB);
		CHECK(childFamilyB.root == parent);
		CHECK(childFamilyB.parent == entity);
		CHECK(childFamilyB.itself == childB);
		CHECK(childFamilyB.siblings.size() == 1);
		CHECK(childFamilyB.children.size() == 0);
	}
};
