#include <bits/stdc++.h>
#include <doctest/doctest.h>
#include "source/exercise/check_vector.h"

/**
 * You are given a string of n characters s[1 . . . n], which you believe to be
 * a corrupted text document in which all punctuation has vanished (so that it
 * looks something like “itwasthebestoftimes...”). You wish to reconstruct the
 * document using a dictionary, which is available in the form of a Boolean
 * function dict(·): for any string w,
 *
 * dict(w) = { true if w is a valid word false otherwise.
 *
 * - Give a dynamic programming algorithm that determines whether the string
 *   s[·] can be reconstituted as a sequence of valid words. The running time
 *   should be at most O(n^2), assuming calls to dict take unit time.
 * - In the event that the string is valid, make your algorithm output the
 *   corresponding sequence of words.
 *
 * Basically the same as 6.2 but with different datatypes.
 *
 * Time complexity: O(n^2)
 * Space complexity: O(n)
 */
std::vector<std::string> restore_whitespace(std::unordered_set<std::string> dictionary, std::string sentence)
{
    int n = sentence.size();
    auto df = std::vector<bool>(n + 1, false);
    auto path = std::vector<int>(n + 1, 0);
    df[0] = true;
    for (int i = 1; i <= n; i++) {
        for (int j = 0; j < i; j++) {
            if (df[j] && dictionary.count(sentence.substr(j, i - j)) > 0) {
                df[i] = true;
                path[i] = j;
                break;
            }
        }
    }
    std::vector<std::string> result;
    if (df[n]) {
        int idx = n;
        while (idx > 0) {
            int wordIdx = path[idx];
            result.push_back(sentence.substr(wordIdx, idx - wordIdx));
            idx = wordIdx;
        }
        std::reverse(result.begin(), result.end());
    }
    return result;
}

TEST_CASE("restore_whitespace")
{
	std::unordered_set<std::string> dictionary = {
		"it",
		"was",
		"the",
		"best",
		"of",
		"times",
		"itwa",
		"super",
		"nice"
	};

	SUBCASE("valid words")
	{
		std::string sentence = "itwasthebestoftimesitwastheof";
		std::vector<std::string> expected = {
			"it", "was", "the", "best", "of", "times", "it", "was", "the", "of"
		};
		CHECK_VECTOR_EQUALS(restore_whitespace(dictionary, sentence), expected);
	}

	SUBCASE("invalid words")
	{
		std::string sentence = "fooitwasthebestoftimesitwastheof";
		std::vector<std::string> expected = {};
		CHECK_VECTOR_EQUALS(restore_whitespace(dictionary, sentence), expected);
	}

	SUBCASE("similar words")
	{
		std::string sentence = "itwasupernice";
		std::vector<std::string> expected = {
			"itwa", "super", "nice"
		};
		CHECK_VECTOR_EQUALS(restore_whitespace(dictionary, sentence), expected);
	}
}
