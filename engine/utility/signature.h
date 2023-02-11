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

}
