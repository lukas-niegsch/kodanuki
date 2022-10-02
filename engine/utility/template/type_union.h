#pragma once
#include <tuple>

namespace Kodanuki
{

/**
 * Base case: only non-tuple types.
 */
template <typename ... Ts>
struct type_union
{
	using type = std::tuple<Ts...>;
};

/**
 * Case 1: First type is empty tuple.
 */
template <typename ... Ts>
struct type_union<std::tuple<>, Ts...> : type_union<Ts...>
{};

/**
 * Case 2: First type is non-empty tuple.
 */
template <typename U, typename ... Us, typename ... Ts>
struct type_union<std::tuple<U, Us...>, Ts...> : std::conditional_t<
	(std::is_same_v<U, Us> || ...) || (std::is_same_v<U, Ts> || ...),
	type_union<std::tuple<Us...>, Ts...>,
	type_union<std::tuple<Us...>, Ts..., U>>
{};

/**
 * Calculates the union of the given types.
 *
 * The types must be given as a (nested) std::tuple. The result will yield
 * a tuple which consists of the union of all non-tuple types. The relative
 * order between the types are kept.
 */
template <typename ... T>
using type_union_t = type_union<T...>::type;

}

