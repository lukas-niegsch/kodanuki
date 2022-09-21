#pragma once
#include <tuple>

namespace Kodanuki
{

template <typename ... T>
struct type_list
{
	using tuple = std::tuple<T...>;
};

template <typename ... Ts>
struct type_union
{
	using type = type_list<Ts...>;
};

template <typename ... Ts>
struct type_union<type_list<>, Ts...> : type_union<Ts...> {};

template <typename U, typename ... Us, typename ... Ts>
struct type_union<type_list<U, Us...>, Ts...> : std::conditional_t<
	(std::is_same_v<U, Us> || ...) || (std::is_same_v<U, Ts> || ...),
	type_union<type_list<Us...>, Ts...>,
	type_union<type_list<Us...>, Ts..., U>> {};

template <typename ... T>
using type_union_t = type_union<T...>::type;

}

