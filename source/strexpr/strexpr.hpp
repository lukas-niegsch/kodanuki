/*******************************************************************************
 *                  ____  ____  ____  ____  _  _  ____  ____
 *                 / ___)(_  _)(  _ \(  __)( \/ )(  _ \(  _ \
 *                 \___ \  )(   )   / ) _)  )  (  ) __/ )   /
 *                 (____/ (__) (__\_)(____)(_/\_)(__)  (__\_)
 *
 * A simple library for processing strings and string expressions.
 *
 * MIT License <https://opensource.org/licenses/MIT>
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2023 Lukas Niegsch
 ******************************************************************************/
#pragma once
#include <cmath>
#include <cstdint>
#include <functional>
#include <optional>
#include <vector>

#include <iterator>
#include <bitset>
#include <iostream>

template<typename T>
void printVector(const T& t) {
    std::copy(t.cbegin(), t.cend(), std::ostream_iterator<typename T::value_type>(std::cout, ", "));
}


namespace strexpr
{

/**
 * An invalid index for when queries don't contain valid indices (Not an Index).
 */
constexpr std::size_t NaI = std::numeric_limits<std::size_t>::max();

/**
 * A function with one input index returning an index into some data.
 */
using unary_query = std::function<std::size_t(std::size_t)>;

/**
 * A function with two input indices returning an index into some data.
 */
using range_query = std::function<std::size_t(std::size_t, std::size_t)>;

/**
 * Precomputes queries of the form pow(2, n).
 *
 * Constraints
 * ===========
 * 0 <= n <= max_value < 64 (on 64-bit systems)
 *
 * Complexity
 * ==========
 * time (precompute) : O(max_value)
 * time (query)      : O(1)
 * space             : O(max_value)
 *
 * @param max_value The maximum value for n.
 * @return A query that computes pow(2, n).
 */
unary_query pow2_query(std::size_t max_value)
{
	std::vector<std::size_t> powers;
	for (std::size_t x = 1; x <= std::pow(2, max_value); x *= 2) {
		powers.push_back(x);
	}
	unary_query query = [=](std::size_t n) {
		return powers[n];
	};
	return query;
}

/**
 * Precomputes queries of the form floor(log(2, n)).
 *
 * Constraints
 * ===========
 * 0 <= n <= max_value
 *
 * Complexity
 * ==========
 * time (precompute) : O(max_value)
 * time (query)      : O(1)
 * space             : O(max_value)
 *
 * @param max_value The maximum value for n.
 * @return A query that computes pow(2, n).
 */
unary_query log2_query(std::size_t max_value)
{
	std::vector<std::size_t> logarithms;
	for (std::size_t i = 0; i <= max_value; i++) {
		logarithms.push_back(std::bit_width(i) - (i != 0));
	}
	unary_query query = [=](std::size_t n) {
		return logarithms[n];
	};
	return query;
}

/**
 * Finds the previous nearest index in the given order.
 *
 * The input is given as the range [begin, end). For each position
 * we yield the index of the previous nearest in-order element. If
 * there is no in-order element we yield NaI.
 *
 * Example
 * =======
 * If the order is std::less, it yields the nearest smaller value.
 * https://en.wikipedia.org/wiki/All_nearest_smaller_values
 *
 * Complexity
 * ==========
 * time (precompute) : O(N)
 * time (query)      : O(1)
 * space             : O(N)
 *
 * @param numbers The numbers on which to answer queries.
 * @param ordered The comparison operation for the order.
 * @return An index vector into the numbers vectors.
 */
template <std::totally_ordered T, class Compare = std::less<T>>
unary_query prev_ordered_query(const std::vector<T>& numbers, Compare ordered = Compare{})
{
	std::size_t N = numbers.size();
	std::vector<std::size_t> nearest_indices(N, NaI);
	for (std::size_t current = 1; current < N; current++) {
		std::size_t previous = current - 1;
		while (!(previous == NaI || ordered(numbers[previous], numbers[current]))) {
			previous = nearest_indices[previous];
		}
		nearest_indices[current] = previous;
	}
	unary_query query = [=](std::size_t n) {
		return nearest_indices[n];
	};
	return query;
}

/**
 * Finds the next nearest index in the given order.
 *
 * The input is given as the range [begin, end). For each position
 * we yield the index of the next nearest in-order element. If
 * there is no in-order element we yield NaI.
 *
 * Example
 * =======
 * If the order is std::less, it yields the nearest smaller value.
 * https://en.wikipedia.org/wiki/All_nearest_smaller_values
 *
 * Complexity
 * ==========
 * time (precompute) : O(N)
 * time (query)      : O(1)
 * space             : O(N)
 *
 * @param numbers The numbers on which to answer queries.
 * @param ordered The comparison operation for the order.
 * @return An index vector into the numbers vectors.
 */
template <std::totally_ordered T, class Compare = std::less<T>>
unary_query next_ordered_query(const std::vector<T>& numbers, Compare ordered = Compare{})
{
	std::size_t N = numbers.size();
	std::vector<T> reversed_numbers(numbers.rbegin(), numbers.rend());
	unary_query prev_query = prev_ordered_query(reversed_numbers, ordered);
	unary_query next_query = [=](std::size_t n) {
		std::size_t x = prev_query(N - n - 1);
		if (x == NaI) return NaI;
		return N - x - 1;
	};
	return next_query;
}

/**
 * Constructs the cartesian tree for the given numbers.
 *
 * The output vector contains the parent positions of nodes in the
 * cartesian tree such that:
 *     - numbers[out[i]] = "parent of numbers[i]"
 *
 * see also:
 * https://en.wikipedia.org/wiki/Cartesian_tree
 *
 * Constraints
 * ===========
 * 0 <= n < N = numbers.size()
 * Each number must be unique.
 *
 * Complexity
 * ==========
 * time (precompute) : O(N)
 * time (query)      : O(1)
 * space             : O(N)
 *
 * @param numbers The numbers on which to answer queries.
 * @return The index of the parent for each node or -1.
 */
template <std::totally_ordered T>
unary_query cartesian_parent_query(const std::vector<T>& numbers)
{
	unary_query nearest_l2r = prev_ordered_query(numbers, std::less<>{});
	unary_query nearest_r2l = next_ordered_index(numbers, std::less<>{});
	unary_query query = [=](std::size_t n) {
		std::size_t l = nearest_l2r(n);
		std::size_t r = nearest_r2l(n);
		if (l == NaI) return r;
		if (r == NaI) return l;
		return numbers[l] >= numbers[r] ? l : r;
	};
	return query;
}

/**
 * Precomputes queries for solving range minimun queries.
 *
 * This algorithm computes a matrix of size N * log2(N) where
 * N is the amount of numbers. Each entry stores queries of the
 * form RMQ(i, i + 2^k - 1). Then any query can be answered by
 * taking the minimum of two blocks. 
 *
 * Constraints
 * ===========
 * 0 <= i <= j < N
 *
 * Complexity
 * ==========
 * time (precompute) : O(N * log2(N))
 * time (query)      : O(1)
 * space             : O(N * log2(N))
 *
 * @param numbers The numbers on which to answer queries.
 * @return A function that computes RMQ(i, j) of the numbers.
 */
template <std::totally_ordered T>
range_query naive_range_minimum_query(const std::vector<T>& numbers)
{
	std::size_t N    = numbers.size();
	unary_query log2 = log2_query(N);
	std::size_t K    = log2(N);
	unary_query pow2 = pow2_query(K);

	std::vector<std::size_t> rmq_index(N * (K + 1));
	auto at = [=](std::size_t i, std::size_t k){ return i + N * k; };
	for (std::size_t i = 0; i < N; i++) {
		rmq_index[at(i, 0)] = i;
	}
	for (std::size_t k = 1; k <= K; k++) {
		for (std::size_t i = 0; i < N - pow2(k - 1); i++) {
			std::size_t a = rmq_index[at(i, k - 1)];
			std::size_t b = rmq_index[at(i + pow2(k - 1), k - 1)];
			rmq_index[at(i, k)] = numbers[a] <= numbers[b] ? a : b;
		}
	}

	auto query = [=](std::size_t i, std::size_t j) {
		uint64_t s = log2(j - i);
		std::size_t a = rmq_index[at(i, s)];
		std::size_t b = rmq_index[at(j - pow2(s) + 1, s)];
		return numbers[a] <= numbers[b] ? a : b;
	};
	return query;
}

/**
 * Precomputes queries for solving range minimun queries.
 *
 * This algorithm splits the array into s = log2(N) / 2 blocks.
 * For each block we can precompute RMQ using the native approach.
 * This solves RMQ queries for inner blocks, but we cannot answer
 * RMQ queries when i or j is not on the block boundary. For
 * these cases we precompute all possible block queries. These
 * are not too many because of the (±1) constraint. Combining
 * three different queries yields the RMQ for the whole sequence.
 *
 * Constraints
 * ===========
 * 0 <= i <= j < N
 * |numbers[i + 1] - numbers[i]| = 1
 *
 * Complexity
 * ==========
 * time (precompute) : O(N)
 * time (query)      : O(1)
 * space             : O(N)
 *
 * @param numbers The numbers on which to answer queries.
 * @return A function that computes RMQ(i, j) of the numbers.
 */
template <std::totally_ordered T>
range_query oneoff_range_minimum_query(const std::vector<T>& numbers)
{
	if (numbers.size() <= 4) {
		return naive_range_minimum_query(numbers);
	}
	std::size_t N = numbers.size();
	std::size_t S = std::ceil(std::log2(N) / 2);
	std::size_t K = (N + S - 1) / S;

	std::vector<T> block_minima(K);
	std::vector<std::size_t> block_indices(K);
	for (std::size_t k = 0; k < K; k++) {
		T min_value = std::numeric_limits<T>::max();
		std::size_t min_index = NaI;
		for (std::size_t s = 0; s < S && s < N - S * k; s++) {
			if (numbers[s + S * k] < min_value) {
				min_value = numbers[s + S * k];
				min_index = s + S * k;
			}
		}
		block_minima[k] = min_value;
		block_indices[k] = min_index;
	}

	std::unordered_map<std::size_t, std::size_t> seen_query_index;
	std::vector<range_query> inblock_queries(K);
	for (std::size_t k = 0; k < K; k++) {
		std::size_t representative_identifier = 0;
		for (std::size_t s = 0; s < S - 1; s++) {
			representative_identifier *= 2;
			if (s + 1 + S * k >= N) {
				representative_identifier++;
			} else {
				if (numbers[s + 1 + S * k] > numbers[s + S * k]) {
					representative_identifier++;
				}
			}
		}
		if (!seen_query_index.contains(representative_identifier)) {
			std::size_t idx = representative_identifier;
			std::vector<std::size_t> representative(S, N);
			for (std::size_t s = S; s != 0; s--) {
				representative[s - 1] = representative[s] + (idx % 2 == 0) ? 1: -1;
				idx /= 2;
			}
			inblock_queries[k] = naive_range_minimum_query(representative);
			seen_query_index[representative_identifier] = k;
		} else {
			inblock_queries[k] = inblock_queries[seen_query_index[representative_identifier]];
		}
	}
	range_query block_rmq = naive_range_minimum_query(block_minima);

	range_query total_query = [=](std::size_t i, std::size_t j) {
		std::size_t a = i / S;
		std::size_t b = j / S;
		std::size_t l1 = inblock_queries[a](i % S, S - 1);
		std::size_t l2 = block_indices[block_rmq(a, b)];
		std::size_t l3 = inblock_queries[b](0, j % S);
		std::cout << l1 << " " << l2 << " " << l3 << std::endl;
		if (numbers[l1] <= numbers[l2] && numbers[l1] <= numbers[l3]) {
			return l1;
		} else if (numbers[l2] < numbers[l1] && numbers[l2] <= numbers[l3]) {
			return l2;
		} else {
			return l3;
		}
	};
	return total_query;
}

}
