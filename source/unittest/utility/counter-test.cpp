#include "engine/utility/counter.h"
#include <doctest/doctest.h>
#include <set>
#include <thread>
using namespace Kodanuki;

static void insertIds(std::set<uint64_t>& ids, int counter)
{
	for (int i = 0; i < counter; i++)
	{
		ids.insert(count());
	}
}

TEST_CASE("guid tests")
{
	SUBCASE("increasing order")
	{
		uint64_t state = count();
		CHECK(count() == state + 1);
		CHECK(count() == state + 2);
		CHECK(count() == state + 3);
	}

	SUBCASE("multi-threaded uniqueness")
	{
		int counter = 10000;
		std::set<uint64_t> ids;

		std::set<uint64_t> partA;
		std::set<uint64_t> partB;

		std::thread t1(insertIds, std::ref(partA), counter / 2);
		std::thread t2(insertIds, std::ref(partB), counter / 2);

		t1.join();
		t2.join();

		ids.insert(partA.begin(), partA.end());
		ids.insert(partB.begin(), partB.end());

		CHECK(ids.size() == counter);
	}
}
