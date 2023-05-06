#include <bits/stdc++.h>
#include <doctest/doctest.h>

namespace math
{

template <typename T>
constexpr auto lcm(std::vector<T> numbers)
{
	T solution = numbers[0];
	for (auto& number : numbers) {
		solution = std::lcm(solution, number);
	}
	return solution;
}

}

/**
 * Given an unlimited supply of coins of denominations x1, x2, ... , xn, we wish
 * to make change for a value v; that is, we wish to find a set of coins whose
 * total value is v. This might not be possible: for instance, if the
 * denominations are 5 and 10 then we can make change for 15 but not for 12.
 * 
 * Give an O(nv) dynamic-programming algorithm for the following problem.
 * Input: x1, ... , xn; v.
 * Question: Is it possible to make change for v using coins of denominations
 *           x1, ... , xn?
 *
 * Time complexity: O(n * v)
 * Space complexity: O(v)
 */
bool exchange_making(std::vector<int> coins, int target)
{
	int v = target % math::lcm(coins);
	auto df = std::vector<bool>(v + 1, false);
	df[0] = true;
	for (int i = 1; i <= v; i++) {
		for (int coin : coins) {
			if (i - coin >= 0) {
				df[i] = std::max(df[i], df[i - coin]);
			}
		}
	}
	return df[v];
}

TEST_CASE("exchange_making")
{
	std::vector<int> mcnuggets = {6, 9, 20};
	CHECK(exchange_making(mcnuggets, 43) == false);
	CHECK(exchange_making(mcnuggets, 44) == true);

	std::vector<int> mcnuggets_with_childbox = {4, 6, 9, 20};
	CHECK(exchange_making(mcnuggets_with_childbox, 43) == true);
	CHECK(exchange_making(mcnuggets_with_childbox, 11) == false);

	std::vector<int> future_german_coins = {2, 5, 10, 20, 50};
	CHECK(exchange_making(future_german_coins, 3) == false);
	CHECK(exchange_making(future_german_coins, 17) == true);
}
