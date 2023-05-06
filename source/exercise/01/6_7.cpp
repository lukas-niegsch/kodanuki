#include <bits/stdc++.h>
#include <doctest/doctest.h>

/**
 * A subsequence is palindromic if it is the same whether read left to right or
 * right to left. For instance, the sequence
 *     A, C, G, T, G, T, C, A, A, A, A, T, C, G
 * has many palindromic subsequences, including A, C, G, C, A and A, A, A, A (on
 * the other hand, the subsequence A, C, T is not palindromic).
 *
 * Devise an algorithm that takes a sequence x[1 . . . n] and returns the
 * (length of the) longest palindromic subsequence. Its running time should be
 * O(n^2).
 *
 * Time complexity: O(n^2)
 * Space complexity: O(n^2)
 */
int longest_palindromic_subsequence(std::string sentence)
{
	int n = sentence.size();
	auto df = std::vector<std::vector<int>>(n, std::vector<int>(n, -1));
	for (int i = sentence.size() - 1; i >= 0; i--) {
		df[i][i] = 1;
		for (int j = i + 1; j < (int) sentence.size(); j++) {
			if (sentence[i] == sentence[j]) {
				df[i][j] = 2 + df[i + 1][j - 1];
			} else {
				df[i][j] = std::max(df[i + 1][j], df[i][j - 1]);
			}
		}
	}
	return df[0][sentence.length() - 1];
}

TEST_CASE("longest_palindromic_subsequence")
{
	SUBCASE("example")
	{
		std::string sentence = "ACGTGTCAAAATCG";
		CHECK(longest_palindromic_subsequence(sentence) == 7); // ACGTGCA
	}
}
