#pragma once
#include "engine/nekolib/templates/type_name.h"
#include "engine/nekolib/templates/concepts.h"
#include <sstream>
#include <string>


namespace kodanuki
{

/**
 * Converts the given structure to its string representation.
 * 
 * This function will produce a compiler error unless it is specialized.
 * It allows us to gradually add new debug information for the types we
 * need. It may only be used for debugging since we cannot rely upon it.
 * 
 * @param info The structure that will be converted.
 * @return A human readable string representation.
 */
template <typename T>
std::string stringify(T info) = delete; // No debug info supported!

/**
 * Converts printable types to its string representation.
 * 
 * These are all types that can be trivally converted to strings such as
 * int, float, or strings itself. This also makes sure that this function
 * works with types that overload operator<<.
 * 
 * @param info The structure that will be converted.
 * @return A human readable string representation.
 */
template <printable T>
std::string stringify(T info) requires (!iterable<T>)
{
	if constexpr (std::is_convertible_v<T, std::string>) {
		return static_cast<std::string>(info);
	} else {
		std::ostringstream ss;
		ss << info;
		return ss.str();
	}
}

/**
 * Converts iterable types to their string representation.
 * 
 * These include types for which we can iterate such as arrays, vectors,
 * or classes which overwrite begin() / end() methods. It will raise an
 * error if the underlying type cannot be converted to a string.
 * 
 * @param info The structure that will be converted.
 * @return A human readable string representation.
 */
template <iterable T>
std::string stringify(T info)
{
	std::stringstream ss;
	ss << type_name<T>() << " {";
	bool first = true;
	for (auto car : info) {
		if (!first) {
			ss << ", ";
		}
		ss << stringify(car);
		first = false;
	}
	ss << "}";
	return ss.str();
}

}
