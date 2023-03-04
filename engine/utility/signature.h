#pragma once
#include <tuple>

namespace kodanuki
{

namespace // internal
{

template <typename Function, int position, bool reverse>
struct signature {};

template <typename R, typename ... Args, int position, bool reverse>
struct signature<R(*)(Args...), position, reverse>
{
	using type = std::conditional_t<reverse,
		std::tuple_element_t<sizeof...(Args) - position - 1, std::tuple<Args...>>,
		std::tuple_element_t<position, std::tuple<Args...>>
	>;
};

template <typename Function>
struct function_traits {};

template <typename R, typename ... Args>
struct function_traits<R(*)(Args...)>
{
	using return_type = R;
	using params_type = std::tuple<Args...>;
};

}

/**
 * Retrieves the argument type at I from some function type (forward).
 */
template <int I, auto Func>
using forward_signature_t = signature<decltype(Func), I, false>::type;

/**
 * Retrieves the argument type at I from some function type (reverse).
 */
template <int I, auto Func>
using reverse_signature_t = signature<decltype(Func), I, true>::type;

/**
 * Retrieves the return type of some function type.
 */
template <auto Func>
using return_signature_t = function_traits<decltype(Func)>::return_type;

/**
 * Retrieves the param types of some function type.
 */
template <auto Func>
using params_signature_t = function_traits<decltype(Func)>::params_type;

}
