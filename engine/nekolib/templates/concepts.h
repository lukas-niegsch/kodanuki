#pragma once
#include <concepts>
#include <iterator>

namespace kodanuki
{

template <typename T>
concept iterable = requires (T v) { std::begin(v); std::end(v); };

template <typename T>
concept printable = requires (T v, std::ostream& os) { os << v; };

template <typename T>
concept buildable = requires(typename T::Builder builder) {
	{ T::create(builder) } -> std::same_as<T>;
};

}
