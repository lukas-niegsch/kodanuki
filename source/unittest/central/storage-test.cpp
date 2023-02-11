#include "engine/central/storage.h"
#include <doctest/doctest.h>
using namespace kodanuki;

TEST_CASE("entity storage tests")
{
    EntityStorage storage;

	SUBCASE("new storage is empty")
	{
		CHECK(storage.size() == 0);
	}

	SUBCASE("new elements increase the size")
	{
		storage.update(0, 42);
		CHECK(storage.size() == 1);
		storage.update(1, 69);
		CHECK(storage.size() == 2);
	}

	SUBCASE("elements with same identifier don't increase the size")
	{
		storage.update(0, 42);
		storage.update(0, 69);
		CHECK(storage.size() == 1);
	}

	SUBCASE("storage contains new element")
	{
		storage.update(0, 42);
		CHECK(storage.contains(0));
	}

	SUBCASE("storage does not contain element after removing it")
	{
		storage.update(0, 42);
		storage.remove(0);
		CHECK(storage.contains(0) == false);
		CHECK(storage.size() == 0);
	}

	SUBCASE("storage stores identifiers")
	{
		storage.update(0, 42);
		storage.update(1, 69);
		std::set identifiers = storage.identifiers();
		CHECK(identifiers.count(0) == true);
		CHECK(identifiers.count(1) == true);
	}

	SUBCASE("storage identifiers exclude weak bindings by default")
	{
		storage.update(0, 42);
		storage.bind(1, 0, true);
		std::set identifiers = storage.identifiers();
		CHECK(identifiers.count(0) == true);
		CHECK(identifiers.count(1) == false);
	}

	SUBCASE("storage identifiers can include weak bindings")
	{
		storage.update(0, 42);
		storage.bind(1, 0, true);
		std::set identifiers = storage.identifiers(true);
		CHECK(identifiers.count(0) == true);
		CHECK(identifiers.count(1) == true);
	}

	SUBCASE("storage saves element data")
	{
		storage.update(0, 42);
		storage.update(1, 69);
		CHECK(std::any_cast<int>(storage.get(0)) == 42);
		CHECK(std::any_cast<int>(storage.get(1)) == 69);
	}

	SUBCASE("storage can copy elements")
	{
		storage.update(0, 42);
		storage.copy(0, 1);
		CHECK(storage.contains(0) == true);
		CHECK(storage.contains(1) == true);
		CHECK(std::any_cast<int>(storage.get(0)) == 42);
		CHECK(std::any_cast<int>(storage.get(1)) == 42);
	}

	SUBCASE("updating the same element overwrites it")
	{
		storage.update(0, 42);
		CHECK(std::any_cast<int>(storage.get(0)) == 42);
		storage.update(0, 69);
		CHECK(std::any_cast<int>(storage.get(0)) == 69);
	}

	SUBCASE("storage can move elements")
	{
		storage.update(0, 42);
		storage.move(0, 1);
		CHECK(storage.contains(0) == false);
		CHECK(storage.contains(1) == true);
		CHECK(std::any_cast<int>(storage.get(1)) == 42);
	}

	SUBCASE("storage can swap elements")
	{
		storage.update(0, 42);
		storage.update(1, 69);
		storage.swap(0, 1);
		CHECK(storage.contains(0) == true);
		CHECK(storage.contains(1) == true);
		CHECK(std::any_cast<int>(storage.get(0)) == 69);
		CHECK(std::any_cast<int>(storage.get(1)) == 42);
	}

	SUBCASE("storage can bind elements")
	{
		storage.update(1, 42);
		storage.bind(0, 1);
		CHECK(storage.contains(0) == true);
		CHECK(storage.contains(1) == true);
		CHECK(std::any_cast<int>(storage.get(0)) == 42);
		CHECK(std::any_cast<int>(storage.get(1)) == 42);
		storage.update(1, 69);
		CHECK(storage.contains(0) == true);
		CHECK(std::any_cast<int>(storage.get(0)) == 69);
		storage.remove(1);
		CHECK(storage.contains(0) == true);
		CHECK(storage.contains(1) == false);
		CHECK(std::any_cast<int>(storage.get(0)) == 69);
	}

	SUBCASE("size includes weak bindings")
	{
		storage.update(1, 42);
		storage.bind(0, 1, true);
		CHECK(storage.size() == 2);
	}

	SUBCASE("stress testing")
	{
		const int count = 100000;
		for (int i = 0; i < count; i++)
		{
			storage.update(i, i);
		}
		for (int i = count / 3; i <= 2 * count / 3; i++)
		{
			storage.remove(i);
		}

		bool isSparseWorking = true;
		bool isDenseWorking = true;
		for (int i = 0; i < count; i++)
		{
			if (count / 3 <= i && i <= 2 * count / 3)
			{
				isSparseWorking &= !storage.contains(i);
			}
			else
			{
				isSparseWorking &= storage.contains(i);
				isDenseWorking &= std::any_cast<int>(storage.get(i)) == i;
			}
		}
		CHECK(isSparseWorking);
		CHECK(isDenseWorking);
	}
}
