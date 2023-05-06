#include <bits/stdc++.h>
#include <doctest/doctest.h>
#include "source/exercise/check_vector.h"

/**
 * You are going on a long trip. You start on the road at mile post 0. Along the
 * way there are n hotels, at mile posts a1 < a2 < ... < an, where each ai is
 * measured from the starting point. The only places you are allowed to stop are
 * at these hotels, but you can choose which of the hotels you stop at. You must
 * stop at the final hotel (at distance an), which is your destination. You’d
 * ideally like to travel 200 miles a day, but this may not be possible
 * (depending on the spacing of the hotels). If you travel x miles during a day,
 * the penalty for that day is (200 − x)^2. You want to plan your trip so as to
 * minimize the total penalty—that is, the sum, over all travel days, of the
 * daily penalties.
 * 
 * Give an efficient algorithm that determines the optimal sequence of hotels at
 * which to stop.
 *
 * Time complexity: O(n^2)
 * Space complexity: O(n)
 *
 * @param hotels The sorted list of the hotel positions.
 * @return The hotels at which to stay.
 */
template <typename ... Args>
std::vector<bool> find_stays(Args ... hotels)
{
	std::vector<float> pos = {0.0f, hotels...};
	int n = pos.size();
	auto df = std::vector<float>(n, std::numeric_limits<float>::max());
	auto path = std::vector<int>(n, 0);
	auto cost = [](float a, float b){ return std::pow(200 - (b - a), 2); };
	df[0] = 0;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < i; j++) {
			float c = df[j] + cost(pos[j], pos[i]);
			if (c < df[i]) {
				df[i] = c;
				path[i] = j;
			}
		}
	}
	std::vector<bool> stays(n - 1, false);
	int position = n - 1;
	while (position != 0) {
		stays[position - 1] = true;
		position = path[position];
	}
	return stays;
}

TEST_CASE("find_stays")
{
	SUBCASE("don't pick the first hotel")
	{
		auto expected = std::vector<bool>({false, true, true});
		auto prediction = find_stays(198.0f, 201.0f, 401.0f);
		CHECK_VECTOR_EQUALS(expected, prediction);
	}
}
