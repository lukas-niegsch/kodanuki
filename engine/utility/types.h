#pragma once
#include <string_view>

/**
 * Prints the name of the template type.
 * https://stackoverflow.com/questions/81870
 */
template <typename T>
constexpr auto type_name() {
	std::string_view name, prefix, suffix;
#ifdef __clang__
	name = __PRETTY_FUNCTION__;
	prefix = "auto type_name() [T = ";
	suffix = "]";
#elif defined(__GNUC__)
	name = __PRETTY_FUNCTION__;
	prefix = "constexpr auto type_name() [with T = ";
	suffix = "]";
#elif defined(_MSC_VER)
	name = __FUNCSIG__;
	prefix = "auto __cdecl type_name<";
	suffix = ">(void)";
#endif
	name.remove_prefix(prefix.size());
	name.remove_suffix(suffix.size());
	return name;
}

/**
 * Retrieves the final argument type from some function type.
 * https://stackoverflow.com/questions/28509273
 */
template <typename Function>
struct FinalArgument {};
template <typename R, typename ... Args>
struct FinalArgument<R(*)(Args...)>
{
    using type = std::tuple_element_t<sizeof...(Args) - 1, std::tuple<Args...>>;
};
