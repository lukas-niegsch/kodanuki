#include "engine/central/archetype.h"
#include "engine/central/entity.h"
#include <doctest/doctest.h>
using namespace Kodanuki;

TEST_CASE("archetype tests")
{
	struct DataA { int value; };
	struct DataB { int value; };
	struct FlagC {};

	Mapping mapping;
	auto dataA_t = std::type_index(typeid(DataA));
	auto dataB_t = std::type_index(typeid(DataB));
	auto flagC_t = std::type_index(typeid(FlagC));
	mapping[dataA_t] = std::make_unique<EntityStorage>();
	mapping[dataB_t] = std::make_unique<EntityStorage>();
	mapping[flagC_t] = std::make_unique<EntityStorage>();

	// iterate over DataA for intersecting ids of DataA
	using Simple = Archetype<Iterator<DataA>, Predicate<>>;

	// iterate over DataA, DataB for intersecting ids of DataA, DataB, FlagC
	using Complex = Archetype<Iterator<DataA, DataB>, Predicate<FlagC>>;

	SUBCASE("empty iteration ")
	{
		int counter = 0;

		for (auto[a] : Simple::iterate(mapping))
		{
			(void) a;
			counter++;
		}

		CHECK(counter == 0);

		for (auto[a, b] : Complex::iterate(mapping))
		{
			(void) a; (void) b;
			counter++;
		}

		CHECK(counter == 0);
	}

	SUBCASE("simple iteration")
	{
		mapping[dataA_t]->update(0, DataA{5});
		mapping[dataA_t]->update(1, DataA{5});

		int counter = 0;

		for (auto[a] : Simple::iterate(mapping))
		{
			CHECK(a.value == 5);
			counter++;
		}

		CHECK(counter == 2);
	}

	SUBCASE("complex iteration")
	{
		mapping[dataA_t]->update(0, DataA{5});
		mapping[dataA_t]->update(2, DataA{6});
		mapping[dataA_t]->update(5, DataA{7});
		mapping[dataA_t]->update(9, DataA{8});

		mapping[dataB_t]->update(1, DataB{5});
		mapping[dataB_t]->update(3, DataB{5});
		mapping[dataB_t]->update(5, DataB{5});
		mapping[dataB_t]->update(9, DataB{5});

		mapping[flagC_t]->update(5, FlagC{});

		int counter = 0;

		for (auto[a, b] : Complex::iterate(mapping))
		{
			(void) a; (void) b;
			counter++;
		}

		// only id 5 is inside the mapping for all three types
		CHECK(counter == 1);
	}
}
