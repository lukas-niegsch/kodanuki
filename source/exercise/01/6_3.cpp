#include <bits/stdc++.h>
#include <doctest/doctest.h>

/**
 * Yuckdonald’s is considering opening a series of restaurants along Quaint
 * Valley Highway (QVH). The n possible locations are along a straight line,
 * and the distances of these locations from the start of QVH are, in miles and
 * in increasing order, m1, m2, ... , mn. The constraints are as follows:
 * - At each location, Yuckdonald’s may open at most one restaurant. The
 *   expected profit from opening a restaurant at location i is pi, where pi > 0
 *   and i = 1, 2, ... , n.
 * - Any two restaurants should be at least k miles apart, where k is a positive
 *   integer.
 * Give an efficient algorithm to compute the maximum expected total profit
 * subject to the given constraints.
 *
 * Time complexity: O(n^2)
 * Space complexity: O(n)
 */
float find_profit(std::vector<float> locations, std::vector<float> profits, float k)
{
	int n = locations.size();
	auto df = std::vector<float>(n, 0);
	df[0] = profits[0];
	for (int i = 1; i < n; i++) {
		int j = i;
		while (std::abs(locations[i] - locations[j]) < k) {
			j--;
			if (j < 0) {
				break;
			}
		}
		// j = "first index where the locations are big enough or -1"
		float tmp_profit = j >= 0 ? df[j] : 0;
		df[i] = std::max(tmp_profit + profits[i], df[i - 1]);
	}
	return df.back();
}

TEST_CASE("find_profit")
{
	SUBCASE("basic test")
	{
		std::vector<float> locations = {1, 2, 3, 4, 5};
		std::vector<float> profits = {10, 20, 15, 30, 25};
		float k = 1.5;
		CHECK(find_profit(locations, profits, k) == 50.0);
	}

	SUBCASE("one valid locations")
	{
		std::vector<float> locations2 = {1, 2, 3, 4, 5};
		std::vector<float> profits2 = {10, 20, 15, 30, 25};
		float k2 = 10;
		CHECK(find_profit(locations2, profits2, k2) == 30.0);
	}

	SUBCASE("all locations are valid")
	{
		std::vector<float> locations3 = {1, 2, 3, 4, 5};
		std::vector<float> profits3 = {10, 20, 15, 30, 25};
		float k3 = 1;
		CHECK(find_profit(locations3, profits3, k3) == 100.0);
	}
}
