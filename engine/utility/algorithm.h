#pragma once
#include <algorithm>
#include <cstdint>
#include <vector>

namespace Kodanuki
{

/**
 * Returns true iff any of the iterables match in some element.
 * The iterables must be of the same type and are only checked
 * until the minimum length (exclusive).
 * 
 * Example:
 * inputs0 = 3 2 1 4 0
 * inputs1 = 2 3 1 5 7 2
 * inputs2 = 5 1 2 1
 * minimum_length = 2 => returns false
 * minimum_length = 3 => returns true "1 == 1 at index 2"
 * 
 * @param minimum_length The minimum length of each iterable.
 * @param inputs The containers that should be matched.
 * @return Is any of the containers elements matching?
 */
template <typename ... Iterable>
bool is_any_match(uint32_t minimum_length, const Iterable& ... inputs)
{
	std::vector firsts = {std::begin(inputs)...};
	for (uint32_t n = 0; n < minimum_length; n++, firsts[0]++) {
		for (uint32_t i = 1; i < sizeof...(inputs); i++) {
			if (*firsts[0] == *firsts[i]) {
				return true;
			}
			firsts[i]++;
		}
	}
	return false;
}

/**
 * Iterates over all the intersecting elements of the iterables.
 * The iterables must be sorted and of the same type. The output
 * will always be sorted.
 * 
 * Example:
 * inputs0 = 2 3 4 5 7
 * inputs1 = 2 3 5 8 9
 * inputs2 = 3 5 7 8 9
 * output = 3 5
 * 
 * @param output The output iterator over itersecting elements.
 * @param inputs The input iterables that contain elements.
 */
template <typename Iterator, typename ... Iterable,
	typename = typename std::enable_if_t<sizeof...(Iterable) >= 2>>
void sorted_intersection(Iterator output, Iterable ... inputs)
{
	uint32_t length = sizeof...(inputs);
	std::vector firsts = {std::begin(inputs)...};
	std::vector lasts = {std::end(inputs)...};
	uint32_t canditate_index = 0;

	auto greater_equal = [&](auto value) {
		return value >= *firsts[canditate_index];
	};

	advance:
	while (!is_any_match(length, firsts, lasts)) {
		for (uint32_t n = 0; n < firsts.size(); n++) {
			if (n == canditate_index) {
				continue;
			}
			firsts[n] = std::find_if(firsts[n], lasts[n], greater_equal);
			if (*firsts[n] != *firsts[canditate_index]) {
				canditate_index = n;
				goto advance;
			}
		}
		*output++ = *firsts[canditate_index]++;
	}
}

/**
 * Do nothing when no iterables are specified.
 */
template <typename Iterator>
void sorted_intersection(Iterator output)
{
	(void) output;
	return;
}

/**
 * Just copy the values when the there is only one input.
 */
template <typename Iterator, typename Iterable>
void sorted_intersection(Iterator output, Iterable input)
{
	std::copy(input.begin(), input.end(), output);
}

}

