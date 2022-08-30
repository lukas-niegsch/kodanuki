#include "engine/central/entity.h"
#include "engine/central/archetype.h"
#include <doctest/doctest.h>
#include <cmath>
using namespace Kodanuki;

TEST_CASE("basic entity tests")
{
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

	SUBCASE("each entity is small")
	{
		CHECK(sizeof(Entity) == 16);
	}

	SUBCASE("compare entities")
	{
		Entity entityA = ECS::create();
		Entity entityB = ECS::create();
		Entity entityC = entityA;
		Entity invalid;
		Entity another;

		CHECK(entityA != invalid);
		CHECK(entityB != invalid);
		CHECK(entityA == entityC);
		CHECK(entityB != entityC);
		CHECK(another == invalid);
	}

	SUBCASE("new entity has only Entity component")
	{
		Entity entity = ECS::create();
		CHECK(ECS::has<Position>(entity) == false);
		CHECK(ECS::has<Quaternion>(entity) == false);
		CHECK(ECS::has<Entity>(entity) == true);
		CHECK(ECS::get<Entity>(entity) == entity);
	}

	SUBCASE("can check if any entity has component")
	{
		CHECK(ECS::has<Position>() == false);
		Entity entity = ECS::create();
		ECS::update<Position>(entity, {10, 10, 10});
		CHECK(ECS::has<Position>() == true);
	}

	SUBCASE("entity can update components")
	{
		Entity entity = ECS::create();
		ECS::update<Position>(entity, {10, 10, 10});
		ECS::update<Quaternion>(entity, {0.0f, 1.0f, 2.0f, 3.0f});
		CHECK(ECS::has<Position>(entity) == true);
		CHECK(ECS::has<Quaternion>(entity) == true);
	}

	SUBCASE("entities must manually remove components")
	{
		Entity entity = ECS::create();
		ECS::update<Position>(entity, {10, 10, 10});
		// do something
		ECS::remove<Position>(entity);
		CHECK(ECS::has<Position>(entity) == false);
	}

	SUBCASE("multiple removal is possible")
	{
		Entity entity = ECS::create();
		ECS::update<Position>(entity, {10, 10, 10});
		ECS::update<Quaternion>(entity, {1, 1, 1, 1});
		ECS::remove<Position, Quaternion>(entity);
		CHECK(ECS::has<Position>(entity) == false);
		CHECK(ECS::has<Quaternion>(entity) == false);
	}

	SUBCASE("component wide removal is possible")
	{
		Entity entityA = ECS::create();
		Entity entityB = ECS::create();
		ECS::update<Position>(entityA, {10, 10, 10});
		ECS::update<Position>(entityB, {10, 10, 10});
		ECS::remove<Position>();
		CHECK(ECS::has<Position>(entityA) == false);
		CHECK(ECS::has<Position>(entityB) == false);
	}

	SUBCASE("values are saved correctly")
	{
		Entity entity = ECS::create();
		ECS::update<Position>(entity, {10, 11, 12});
		Position& position = ECS::get<Position>(entity);
		CHECK(position.x == 10);
		CHECK(position.y == 11);
		CHECK(position.z == 12);
	}

	SUBCASE("components can be copied")
	{
		Entity entityA = ECS::create();
		Entity entityB = ECS::create();
		ECS::update<Position>(entityA, {10, 10, 10});
		ECS::copy<Position>(entityA, entityB);
		CHECK(ECS::has<Position>(entityA) == true);
		CHECK(ECS::has<Position>(entityB) == true);
		CHECK(ECS::get<Position>(entityB).x == 10);
	}

	SUBCASE("multiple components can be copied at once")
	{
		Entity entityA = ECS::create();
		Entity entityB = ECS::create();
		ECS::update<Position>(entityA, {10, 10, 10});
		ECS::update<Quaternion>(entityA, {1, 1, 1, 1});
		ECS::copy<Position, Quaternion>(entityA, entityB);
		CHECK(ECS::has<Position>(entityA) == true);
		CHECK(ECS::has<Position>(entityB) == true);
		CHECK(ECS::get<Position>(entityB).x == 10);
		CHECK(ECS::has<Quaternion>(entityA) == true);
		CHECK(ECS::has<Quaternion>(entityB) == true);
		CHECK(ECS::get<Quaternion>(entityB).x == 1);
	}

	SUBCASE("components can be moved")
	{
		Entity entityA = ECS::create();
		Entity entityB = ECS::create();
		ECS::update<Position>(entityA, {10, 10, 10});
		ECS::move<Position>(entityA, entityB);
		CHECK(ECS::has<Position>(entityA) == false);
		CHECK(ECS::has<Position>(entityB) == true);
		CHECK(ECS::get<Position>(entityB).x == 10);
	}

	SUBCASE("multiple components can be moved at once")
	{
		Entity entityA = ECS::create();
		Entity entityB = ECS::create();
		ECS::update<Position>(entityA, {10, 10, 10});
		ECS::update<Quaternion>(entityA, {1, 1, 1, 1});
		ECS::move<Position, Quaternion>(entityA, entityB);
		CHECK(ECS::has<Position>(entityA) == false);
		CHECK(ECS::has<Position>(entityB) == true);
		CHECK(ECS::get<Position>(entityB).x == 10);
		CHECK(ECS::has<Quaternion>(entityA) == false);
		CHECK(ECS::has<Quaternion>(entityB) == true);
		CHECK(ECS::get<Quaternion>(entityB).x == 1);
	}

	SUBCASE("components can be swapped")
	{
		Entity entityA = ECS::create();
		Entity entityB = ECS::create();
		Entity entityC = ECS::create();
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

	SUBCASE("multiple components can be swappped at once")
	{
		Entity entityA = ECS::create();
		Entity entityB = ECS::create();
		ECS::update<Position>(entityA, {10, 10, 10});
		ECS::update<Quaternion>(entityA, {1, 1, 1, 1});
		ECS::swap<Position, Quaternion>(entityA, entityB);
		CHECK(ECS::has<Position>(entityA) == false);
		CHECK(ECS::has<Position>(entityB) == true);
		CHECK(ECS::get<Position>(entityB).x == 10);
		CHECK(ECS::has<Quaternion>(entityA) == false);
		CHECK(ECS::has<Quaternion>(entityB) == true);
		CHECK(ECS::get<Quaternion>(entityB).x == 1);
	}

	SUBCASE("components can be bound")
	{
		Entity entityA = ECS::create();
		Entity entityB = ECS::create();
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

	SUBCASE("multiple components can be bound at once")
	{
		Entity entityA = ECS::create();
		Entity entityB = ECS::create();
		ECS::update<Position>(entityB, {10, 10, 10});
		ECS::update<Quaternion>(entityB, {1, 1, 1, 1});
		ECS::bind<Position, Quaternion>(entityA, entityB);
		CHECK(ECS::has<Position>(entityA) == true);
		CHECK(ECS::has<Position>(entityB) == true);
		CHECK(ECS::get<Position>(entityB).x == 10);
		CHECK(ECS::has<Quaternion>(entityA) == true);
		CHECK(ECS::has<Quaternion>(entityB) == true);
		CHECK(ECS::get<Quaternion>(entityB).x == 1);
	}

	// cleanup
	ECS::remove<Position>();
	ECS::remove<Quaternion>();
};

TEST_CASE("entity component iteration tests")
{
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

	using Positions = Archetype<Iterator<Position>, Predicate<>>;
	using Rotations = Archetype<Iterator<Position, Quaternion>, Predicate<>>;

	SUBCASE("iteration with one parameter")
	{
		Entity entityA = ECS::create();
		Entity entityB = ECS::create();
		ECS::update<Position>(entityA, {10, 10, 10});
		ECS::update<Position>(entityB, {10, 10, 10});

		int counter = 0;
		for (auto [position] : ECS::iterate<Positions>())
		{
			CHECK(position.x == 10);
			counter++;
		}
		CHECK(counter == 2);
	}

	SUBCASE("iteration with multiple parameters")
	{
		Entity entityA = ECS::create();
		Entity entityB = ECS::create();
		Entity entityC = ECS::create();
		Entity entityD = ECS::create();
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

	SUBCASE("iteration can ignore weak bindings")
	{
		Entity entityA = ECS::create();
		Entity entityB = ECS::create();
		Entity entityC = ECS::create();
		ECS::update<Position>(entityA, {10, 10, 10});
		ECS::bind<Position>(entityB, entityA);
		ECS::bind<Position>(entityC, entityA);
		ECS::update<Quaternion>(entityA, {1.0, 1.0, 1.0, 1.0});
		ECS::bind<Quaternion>(entityB, entityA, false);
		ECS::bind<Quaternion>(entityC, entityA, true);

		// now all entities have both Position and Quaternion
		// but the component Quaternion is bound weak to entityC

		int counter = 0; 
		for (auto[position, rotation] : ECS::iterate<Rotations>(false))
		{
			CHECK(position.x == 10);
			CHECK(std::abs(rotation.w - 1.0) <= 0.001);
			counter++;
		}
		CHECK(counter == 2); // only not weak bindings are considered

		counter = 0; 
		for (auto[position, rotation] : ECS::iterate<Rotations>(true))
		{
			CHECK(position.x == 10);
			CHECK(std::abs(rotation.w - 1.0) <= 0.001);
			counter++;
		}
		CHECK(counter == 3); // weak bindings are considered too
	}

	SUBCASE("iteration can modify elements")
	{
		Entity entity = ECS::create();
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

	SUBCASE("rebinding can remove weakness")
	{
		Entity main = ECS::create();
		ECS::update<Position>(main, {10, 10, 10});
		ECS::update<Quaternion>(main, {1.0, 1.0, 1.0, 1.0});

		Entity entity = ECS::create();
		ECS::bind<Position>(entity, main, true);
		ECS::bind<Quaternion>(entity, main, true);

		int counter = 0;
		for (auto[position, rotation] : ECS::iterate<Rotations>())
		{
			(void) position;
			(void) rotation;
			counter++;
		}
		CHECK(counter == 1);

		ECS::bind<Position>(entity, main);
		ECS::bind<Quaternion>(entity, main);

		counter = 0;
		for (auto[position, rotation] : ECS::iterate<Rotations>())
		{
			(void) position;
			(void) rotation;
			counter++;
		}
		CHECK(counter == 2);
	}

	ECS::remove<Position>();
	ECS::remove<Quaternion>();
}
