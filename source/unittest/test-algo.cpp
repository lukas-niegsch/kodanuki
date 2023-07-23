#include "engine/nekolib/container/vebtree.h"
#include "engine/nekolib/algorithm/vebsort.h"
#include <doctest/doctest.h>
#include <bits/stdc++.h>
using namespace kodanuki;

TEST_CASE("Vebtree Tests")
{
    SUBCASE("Van Emde Boas Tree API usage with Integers")
    {
        Vebtree<uint16_t, 256> tree;
        CHECK(tree.get_min() == std::nullopt);

        tree.insert(32);
        tree.insert(64);
        tree.insert(47);
        CHECK(tree.get_min() != std::nullopt);
        CHECK(tree.contains(32) == true);
        CHECK(tree.contains(33) == false);

        tree.remove(64);
        CHECK(tree.contains(64) == false);

        CHECK(tree.get_min() == 32);
        CHECK(tree.get_max() == 47);

        CHECK(tree.get_prev(31) == std::nullopt);
        CHECK(tree.get_prev(32) == std::nullopt);
        CHECK(tree.get_prev(33) == 32);
        CHECK(tree.get_prev(43) == 32);
        CHECK(tree.get_prev(99) == 47);

        CHECK(tree.get_next(48) == std::nullopt);
        CHECK(tree.get_next(47) == std::nullopt);
        CHECK(tree.get_next(46) == 47);
        CHECK(tree.get_next(37) == 47);
        CHECK(tree.get_next(12) == 32);

        tree.remove(32);
        tree.remove(64);
        tree.remove(47);
        CHECK(tree.get_min() == std::nullopt);
    }

    /**
     * Honestly, doing this with strings seems like a terrible idea. The
     * problem is that we need O(M) space where M is the largest possible
     * value of order(s), where s is some custom type. For strings we get
     * a space complexity of O(sizeof(char) ^ max_string_size). Even if
     * we limit the alphabet, we are still exponential in the length of
     * the string. Instead of strings, we could consider any countable type
     * with an upper limit.
     */
    SUBCASE("Van Emde Boas Tree API usage with Strings")
    {
        // There must be an upper limit to the size!
        constexpr int max_string_size = 2;

        // The order must map the type uniquely to ints.
        constexpr auto string_order = [](std::string x) {
            assert(x.size() <= max_string_size);
            uint64_t hash = 0;
            for (char c : x) {
                hash = hash * 256 + static_cast<uint64_t>(c);
            }
            return hash;
        };

        // The order is first smallest length then lexicographical order.
        CHECK(string_order("A") == 65);
        CHECK(string_order("B") == 66);
        CHECK(string_order("C") == 67);
        CHECK(string_order("AA") == 16705);
        CHECK(string_order("AB") == 16706);
        CHECK(string_order("BA") == 16961);
        
        // The tree uses this order to compare the custom type.
        constexpr uint64_t size = 1ULL << (8 * sizeof(char) * max_string_size);
        Vebtree<std::string, size, string_order> tree;

        // Afterward, we can use the Vebtree normally!
        CHECK(tree.get_min() == std::nullopt);

        tree.insert("A");
        tree.insert("AC");
        tree.insert("CA");

        CHECK(tree.get_min() != std::nullopt);
        CHECK(tree.contains("A") == true);
        CHECK(tree.contains("CA") == true);
        
        tree.remove("CA");
        CHECK(tree.contains("CA") == false);

        CHECK(tree.get_min() == "A");
        CHECK(tree.get_max() == "AC");

        CHECK(tree.get_prev("6") == std::nullopt);
        CHECK(tree.get_prev("A") == std::nullopt);
        CHECK(tree.get_prev("B") == "A");
        CHECK(tree.get_prev("F") == "A");
        CHECK(tree.get_prev("FC") == "AC");

        CHECK(tree.get_next("AD") == std::nullopt);
        CHECK(tree.get_next("AC") == std::nullopt);
        CHECK(tree.get_next("AB") == "AC");
        CHECK(tree.get_next("F") == "AC");
        CHECK(tree.get_next("6") == "A");

        tree.remove("A");
        tree.remove("AC");
        tree.remove("CA");
        CHECK(tree.get_min() == std::nullopt);
    }

    SUBCASE("new trees are empty")
    {
        Vebtree<uint16_t, 256> tree;
        CHECK(tree.get_min() == std::nullopt);
    }

    SUBCASE("after one insert tree is not empty")
    {
        Vebtree<uint16_t, 256> tree;
        tree.insert(0);
        CHECK(tree.get_min() != std::nullopt);
    }

    SUBCASE("tree contains inserted elements")
    {
        Vebtree<uint16_t, 256> tree;
        tree.insert(123);
        CHECK(tree.contains(123) == true);
    }

    SUBCASE("inserting multiple elements works")
    {
        Vebtree<uint16_t, 256> tree;
        tree.insert(5);
        tree.insert(123);
        tree.insert(56);
        CHECK(tree.contains(5) == true);
        CHECK(tree.contains(123) == true);
        CHECK(tree.contains(56) == true);
    }

    SUBCASE("tree does not contain elements which have not been inserted")
    {
        Vebtree<uint16_t, 256> tree;
        CHECK(tree.contains(0) == false);
        CHECK(tree.contains(421) == false);
        CHECK(tree.contains(40) == false);
    }

    SUBCASE("empty tree does not contain min and max values")
    {
        Vebtree<uint16_t, 256> tree;
        CHECK(tree.get_min() == std::nullopt);
        CHECK(tree.get_max() == std::nullopt);
    }

    SUBCASE("min and max value are equal inside a single element tree")
    {
        Vebtree<uint16_t, 256> tree;
        tree.insert(15);
        CHECK(tree.get_min() == 15);
        CHECK(tree.get_max() == 15);
    }

    SUBCASE("min and max values are updated on insertion")
    {
        Vebtree<uint16_t, 256> tree;
        tree.insert(123);
        CHECK(tree.get_min() == 123);
        CHECK(tree.get_max() == 123);
        tree.insert(36);
        CHECK(tree.get_min() == 36);
        CHECK(tree.get_max() == 123);
        tree.insert(215);
        CHECK(tree.get_min() == 36);
        CHECK(tree.get_max() == 215);
        tree.insert(47);
        CHECK(tree.get_min() == 36);
        CHECK(tree.get_max() == 215);
        tree.insert(24);
        CHECK(tree.get_min() == 24);
        CHECK(tree.get_max() == 215);
    }

    SUBCASE("after one insert and one remove tree is empty")
    {
        Vebtree<uint16_t, 256> tree;
        tree.insert(215);
        tree.remove(215);
        CHECK(tree.get_min() == std::nullopt);
    }

    SUBCASE("removing twice is possible")
    {
        Vebtree<uint16_t, 256> tree;
        tree.insert(67);
        tree.remove(67);
        tree.remove(67);
        CHECK(tree.contains(67) == false);
    }

    SUBCASE("inserting twice is possible")
    {
        Vebtree<uint16_t, 256> tree;
        tree.insert(67);
        tree.insert(67);
        tree.remove(67);
        CHECK(tree.contains(67) == false);
    }

    SUBCASE("removing one elements does not affect another")
    {
        Vebtree<uint16_t, 256> tree;
        tree.insert(67);
        tree.insert(49);
        tree.remove(67);
        CHECK(tree.contains(67) == false);
        CHECK(tree.contains(49) == true);
    }

    SUBCASE("min and max are updated after removing elements")
    {
        Vebtree<uint16_t, 256> tree;
        tree.insert(24);
        tree.insert(36);
        tree.insert(47);
        tree.insert(123);
        tree.insert(215);
        CHECK(tree.get_min() == 24);
        CHECK(tree.get_max() == 215);
        tree.remove(24);
        CHECK(tree.get_min() == 36);
        CHECK(tree.get_max() == 215);
        tree.remove(47);
        CHECK(tree.get_min() == 36);
        CHECK(tree.get_max() == 215);
        tree.remove(215);
        CHECK(tree.get_min() == 36);
        CHECK(tree.get_max() == 123);
        tree.remove(36);
        CHECK(tree.get_min() == 123);
        CHECK(tree.get_max() == 123);
        tree.remove(123);
        CHECK(tree.get_min() == std::nullopt);
        CHECK(tree.get_max() == std::nullopt);
    }

    SUBCASE("next value is empty for empty tree")
    {
        Vebtree<uint16_t, 256> tree;
        CHECK(tree.get_next(17) == std::nullopt);
        CHECK(tree.get_next(42) == std::nullopt);
    }

    SUBCASE("prev value is empty for empty tree")
    {
        Vebtree<uint16_t, 256> tree;
        CHECK(tree.get_prev(17) == std::nullopt);
        CHECK(tree.get_prev(42) == std::nullopt);
    }

    SUBCASE("next value works with single element trees")
    {
        Vebtree<uint16_t, 256> tree;
        tree.insert(64);
        CHECK(tree.get_next(17) == 64);
        CHECK(tree.get_next(63) == 64);
        CHECK(tree.get_next(64) == std::nullopt);
        CHECK(tree.get_next(65) == std::nullopt);
        CHECK(tree.get_next(99) == std::nullopt);
    }

    SUBCASE("prev value works with single element trees")
    {
        Vebtree<uint16_t, 256> tree;
        tree.insert(64);
        CHECK(tree.get_prev(17) == std::nullopt);
        CHECK(tree.get_prev(63) == std::nullopt);
        CHECK(tree.get_prev(64) == std::nullopt);
        CHECK(tree.get_prev(65) == 64);
        CHECK(tree.get_prev(99) == 64);
    }

    SUBCASE("next value works with multiple element trees")
    {
        Vebtree<uint16_t, 256> tree;
        tree.insert(24);
        tree.insert(36);
        tree.insert(47);
        CHECK(tree.get_next(23) == 24);
        CHECK(tree.get_next(24) == 36);
        CHECK(tree.get_next(25) == 36);
        CHECK(tree.get_next(35) == 36);
        CHECK(tree.get_next(36) == 47);
        CHECK(tree.get_next(37) == 47);
        CHECK(tree.get_next(46) == 47);
        CHECK(tree.get_next(47) == std::nullopt);
        CHECK(tree.get_next(48) == std::nullopt);
    }

    SUBCASE("prev value works with multiple element trees")
    {
        Vebtree<uint16_t, 256> tree;
        tree.insert(24);
        tree.insert(36);
        tree.insert(47);
        CHECK(tree.get_prev(23) == std::nullopt);
        CHECK(tree.get_prev(24) == std::nullopt);
        CHECK(tree.get_prev(25) == 24);
        CHECK(tree.get_prev(35) == 24);
        CHECK(tree.get_prev(36) == 24);
        CHECK(tree.get_prev(37) == 36);
        CHECK(tree.get_prev(46) == 36);
        CHECK(tree.get_prev(47) == 36);
        CHECK(tree.get_prev(48) == 47);
    }

    SUBCASE("vebtree based sorting works for only for unique values")
    {
        std::vector<uint16_t> values = {
            4, 7, 12, 3, 96, 77, 88, 34, 52
        };
        vebsort<256>(values);
        REQUIRE(values.size() == 9);
        CHECK(values[0] == 3);
        CHECK(values[1] == 4);
        CHECK(values[2] == 7);
        CHECK(values[3] == 12);
        CHECK(values[4] == 34);
        CHECK(values[5] == 52);
        CHECK(values[6] == 77);
        CHECK(values[7] == 88);
        CHECK(values[8] == 96);
    }
}
