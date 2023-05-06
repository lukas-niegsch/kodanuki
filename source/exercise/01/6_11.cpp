#include <bits/stdc++.h>
#include <doctest/doctest.h>

/**
 * Given two strings x = x1 x2 ... xn and y = y1 y2 ... ym, we wish to find the
 * length of their longest common subsequence, that is, the largest k for which
 * there are indices i1 < i2 < ... < ik and j1 < j2 < ... < jk with
 * xi1 xi2 ... xik = yj1 yj2 ... yjk . Show how to do this in time O(mn).
 *
 * Time complexity: O(n * m)
 * Space complexity: O(m)
 */
int longest_common_subsequence(std::string x, std::string y)
{
	int n = x.size();
	int m = y.size();
	auto df = std::vector<std::vector<int>>(2, std::vector<int>(m + 1, 0));
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			int v;
			if (x[i] == y[j]) {
				v = df[i % 2 == 1][j] + 1;
			} else {
				v = std::max(
					df[i % 2 == 1][j + 1],
					df[i % 2 == 0][j]
				);
			}
			df[i % 2 == 0][j + 1] = v;
		}
	}
	return df[n % 2 == 1][m];
}

TEST_CASE("longest_common_subsequence")
{
	std::string x = "Hello, World x";
	std::string y = "Yo, Wordle x";
	CHECK(longest_common_subsequence(x, y) == 9);
}
