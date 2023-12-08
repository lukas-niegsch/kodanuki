#pragma once
#include <array>
#include <concepts>
#include <type_traits>


namespace kodanuki
{

template <std::integral ... T> 
[[nodiscard]] constexpr auto suffix_array(const T& ... input_word) noexcept
{
	using Q = std::common_type<T...>;
	constexpr std::size_t n = sizeof...(T); 
	return std::array<Q, n>();
}

}
