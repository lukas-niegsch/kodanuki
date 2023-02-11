#include "engine/central/archetype.h"
#include "engine/central/entity.h"
#include "engine/central/family.h"
#include <doctest/doctest.h>
#include <cmath>
using namespace kodanuki;

struct Position
{
	int x;
	int y;
	int z;
};

struct Quaternion
{
	float w;
	float x;
	float y;
	float z;
};

TEST_CASE("basic entity tests")
{
	// setup
	Entity entity = ECS::create();
	Entity entityA = ECS::create();
	Entity entityB = ECS::create();
	Entity entityC = ECS::create();

	SUBCASE("each entity is small")
	{
		CHECK(sizeof(Entity) == 16);
	}

	SUBCASE("compare entities")
	{
		Entity copied = entityA;
		Entity invalid;
		Entity another;

		CHECK(entityA != invalid);
		CHECK(entityB != invalid);
		CHECK(entityC != invalid);

		CHECK(entityA != entityB);
		CHECK(entityA != entityC);
		CHECK(entityB != entityC);

		CHECK(entityA == copied);
		CHECK(another == invalid);
	}

	SUBCASE("new entities only have entity component")
	{
		CHECK(ECS::has<Position>(entity) == false);
		CHECK(ECS::has<Quaternion>(entity) == false);
		CHECK(ECS::has<Family>(entity) == false);
		CHECK(ECS::has<Entity>(entity) == true);
		CHECK(ECS::get<Entity>(entity) == entity);
	}

	SUBCASE("entity can update components")
	{
		ECS::update<Position>(entity, {10, 10, 10});
		ECS::update<Quaternion>(entity, {0.0f, 1.0f, 2.0f, 3.0f});
		CHECK(ECS::has<Position>(entity) == true);
		CHECK(ECS::has<Quaternion>(entity) == true);
	}

	SUBCASE("entities can remove components")
	{
		ECS::update<Position>(entity, {10, 10, 10});
		// do something
		ECS::remove<Position>(entity);
		CHECK(ECS::has<Position>(entity) == false);
	}

	SUBCASE("entities can be removed and all its components")
	{
		ECS::update<Position>(entity, {10, 10, 10});
		ECS::update<Quaternion>(entity, {1, 1, 1, 1});
		ECS::remove<Entity>(entity);
		CHECK(ECS::has<Position>(entity) == false);
		CHECK(ECS::has<Quaternion>(entity) == false);
	}

	SUBCASE("values are saved correctly")
	{
		ECS::update<Position>(entity, {10, 11, 12});
		Position& position = ECS::get<Position>(entity);
		CHECK(position.x == 10);
		CHECK(position.y == 11);
		CHECK(position.z == 12);
	}

	SUBCASE("components can be copied")
	{
		ECS::update<Position>(entityA, {10, 10, 10});
		ECS::copy<Position>(entityA, entityB);
		CHECK(ECS::has<Position>(entityA) == true);
		CHECK(ECS::has<Position>(entityB) == true);
		CHECK(ECS::get<Position>(entityB).x == 10);
	}

	SUBCASE("components can be moved")
	{
		ECS::update<Position>(entityA, {10, 10, 10});
		ECS::move<Position>(entityA, entityB);
		CHECK(ECS::has<Position>(entityA) == false);
		CHECK(ECS::has<Position>(entityB) == true);
		CHECK(ECS::get<Position>(entityB).x == 10);
	}

	SUBCASE("components can be swapped")
	{
		ECS::update<Position>(entityA, {10, 10, 10});
		ECS::update<Position>(entityB, {20, 20, 20});
		ECS::swap<Position>(entityA, entityB);
		CHECK(ECS::has<Position>(entityA) == true);
		CHECK(ECS::has<Position>(entityB) == true);
		CHECK(ECS::get<Position>(entityA).x == 20);
		CHECK(ECS::get<Position>(entityB).x == 10);
		ECS::swap<Position>(entityB, entityC);
		CHECK(ECS::has<Position>(entityB) == false);
		CHECK(ECS::has<Position>(entityC) == true);
		CHECK(ECS::get<Position>(entityC).x == 10);
	}

	SUBCASE("components can be bound")
	{
		ECS::update<Position>(entityB, {10, 10, 10});
		ECS::bind<Position>(entityA, entityB);
		CHECK(ECS::has<Position>(entityA) == true);
		CHECK(ECS::has<Position>(entityB) == true);
		CHECK(ECS::get<Position>(entityA).x == 10);
		CHECK(ECS::get<Position>(entityB).x == 10);
		ECS::update<Position>(entityB, {20, 20, 20});
		CHECK(ECS::get<Position>(entityA).x == 20);
		CHECK(ECS::get<Position>(entityB).x == 20);
		ECS::update<Position>(entityA, {30, 30, 30});
		CHECK(ECS::get<Position>(entityA).x == 30);
		CHECK(ECS::get<Position>(entityB).x == 30);
		ECS::remove<Position>(entityB);
		CHECK(ECS::has<Position>(entityA) == true);
		CHECK(ECS::has<Position>(entityB) == false);
	}

	// cleanup
	ECS::remove<Entity>(entity);
	ECS::remove<Entity>(entityA);
	ECS::remove<Entity>(entityB);
	ECS::remove<Entity>(entityC);
};

TEST_CASE("entity component iteration tests")
{
	Entity entity = ECS::create();
	Entity entityA = ECS::create();
	Entity entityB = ECS::create();
	Entity entityC = ECS::create();
	Entity entityD = ECS::create();

	using Positions = Archetype<Iterate<Position>>;
	using Rotations = Archetype<Iterate<Position, Quaternion>>;

	SUBCASE("iteration with one parameter")
	{
		ECS::update<Position>(entityA, {10, 10, 10});
		ECS::update<Position>(entityB, {10, 10, 10});

		int counter = 0;
		for (auto[position] : ECS::iterate<Positions>())
		{
			CHECK(position.x == 10);
			counter++;
		}
		CHECK(counter == 2);
	}

	SUBCASE("iteration with multiple parameters")
	{
		ECS::update<Position>(entityA, {10, 10, 10});
		ECS::update<Position>(entityB, {10, 10, 10});
		ECS::bind<Position>(entityC, entityB);
		ECS::update<Quaternion>(entityA, {1.0f, 1.0f, 1.0f, 1.0f});
		ECS::update<Quaternion>(entityC, {1.0f, 1.0f, 1.0f, 1.0f});
		ECS::update<Quaternion>(entityD, {1.0f, 1.0f, 1.0f, 1.0f});

		int counter = 0; 
		for (auto[position, rotation] : ECS::iterate<Rotations>())
		{
			CHECK(position.x == 10);
			CHECK(std::abs(rotation.w - 1.0) <= 0.001);
			counter++;
		}
		CHECK(counter == 2); // elements that have all queried components: A, C	
	}

	SUBCASE("iteration can modify elements")
	{
		ECS::update<Position>(entity, {10, 10, 10});
		ECS::update<Quaternion>(entity, {1.0, 1.0, 1.0, 1.0});

		for (auto [position] : ECS::iterate<Positions>())
		{
			position.x = 20;
		}

		CHECK(ECS::get<Position>(entity).x == 20);
		CHECK(ECS::get<Position>(entity).y == 10);
		CHECK(ECS::get<Position>(entity).z == 10);

		for (auto[position, rotation] : ECS::iterate<Rotations>())
		{
			position.y++;
			rotation.w = 2.0;
		}

		CHECK(ECS::get<Position>(entity).y == 11);
		CHECK(std::abs(ECS::get<Quaternion>(entity).w - 2.0) <= 0.001);
	}

	ECS::remove<Entity>(entity);
	ECS::remove<Entity>(entityA);
	ECS::remove<Entity>(entityB);
	ECS::remove<Entity>(entityC);
	ECS::remove<Entity>(entityD);
}

template <typename Archetype>
int iteration_counter()
{
	int counter = 0;
	for (auto[position, rotation] : ECS::iterate<Archetype>()) {
		(void) position;
		(void) rotation;
		counter++;
	}
	return counter;
}

TEST_CASE("archetype iteration predicate tests")
{
	struct FlagA {};
	struct FlagB {};
	struct FlagC {};
	
	auto create_default_entity = []{
		Entity entity = ECS::create();
		ECS::update<Position>(entity, {});
		ECS::update<Quaternion>(entity, {});
		return entity;
	};

	Entity entityA = create_default_entity();
	Entity entityB = create_default_entity();
	Entity entityC = create_default_entity();
	Entity entityD = create_default_entity();
	Entity entityE = create_default_entity();

	// Always iterate over entities with postion and quaterion component.
	using I = Iterate<Position, Quaternion>;
	
	SUBCASE("simple iteration without any additional predicates")
	{
		using System = Archetype<I>;

		static_assert(std::is_same_v<System::iterate_types,
			std::tuple<Position, Quaternion>
		>);

		// All 5 entities have position and quaternion components.
		CHECK(iteration_counter<System>() == 5);

		ECS::remove<Position>(entityA);
		ECS::remove<Quaternion>(entityB);

		// Now only 3 of them have position AND quaternion components.
		CHECK(iteration_counter<System>() == 3);
	}
	
	SUBCASE("iteration with additional required flags")
	{
		using System = Archetype<I, Require<FlagA, FlagB>>;
		
		static_assert(std::is_same_v<System::include_types,
			std::tuple<Position, Quaternion, FlagA, FlagB>
		>);

		// No entity has FlagA and FlagB.
		CHECK(iteration_counter<System>() == 0);

		ECS::update<FlagA>(entityA);
		ECS::update<FlagB>(entityA);
		ECS::update<FlagA>(entityB);

		// Only entityA has both flags.
		CHECK(iteration_counter<System>() == 1);
	}

	SUBCASE("iteration with additional exluded flags")
	{
		using System = Archetype<I, Require<FlagA>, Exclude<FlagB>>;

		ECS::update<FlagA>(entityA);
		ECS::update<FlagA>(entityB);
		
		CHECK(iteration_counter<System>() == 2);

		ECS::update<FlagA>(entityC);
		
		CHECK(iteration_counter<System>() == 3);

		ECS::update<FlagB>(entityD);
		ECS::update<FlagB>(entityA);

		CHECK(iteration_counter<System>() == 2);
	}
	
	SUBCASE("useless iteration predicate combination")
	{
		using System = Archetype<I, Exclude<Position>>;
		
		// Include Postion + Exclude Position = Zero Iteration
		CHECK(iteration_counter<System>() == 0);
	}

	SUBCASE("consume iteration predicate")
	{
		using System = Archetype<I, Consume<FlagA, FlagB>>;

		// Consume flags must be included.
		CHECK(iteration_counter<System>() == 0);

		ECS::update<FlagA>(entityA);
		ECS::update<FlagB>(entityA);
		ECS::update<FlagB>(entityB);

		// Only entityA has both consume flags
		CHECK(iteration_counter<System>() == 1);

		CHECK(ECS::has<FlagA>(entityA) == false);
		CHECK(ECS::has<FlagB>(entityA) == false);
		CHECK(ECS::has<FlagB>(entityB) == true);

		// Good for system that wait for other systems to produce flags.
		CHECK(iteration_counter<System>() == 0);
	}

	SUBCASE("produce iteration predicate")
	{
		using System = Archetype<I, Produce<FlagA>>;

		CHECK(iteration_counter<System>() == 5);
		CHECK(iteration_counter<System>() == 0);

		ECS::remove<FlagA>(entityA);
		ECS::remove<FlagA>(entityE);

		CHECK(iteration_counter<System>() == 2);
		CHECK(iteration_counter<System>() == 0);
	}

	ECS::remove<Entity>(entityA);
	ECS::remove<Entity>(entityB);
	ECS::remove<Entity>(entityC);
	ECS::remove<Entity>(entityD);
	ECS::remove<Entity>(entityE);
}

