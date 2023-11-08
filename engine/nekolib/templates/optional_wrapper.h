#pragma once
#include <iostream>
#include <optional>
#include <string>


namespace kodanuki
{

template <typename T>
struct OptionalWrapper
{
	std::optional<T> optional;
	std::string error_reason;
	std::string error_result;

	T expect(std::string message)
	{
		if (!optional) {
			std::cout << message << '\n';
			std::cout << "[Error] because: " << error_result << '\n';
			std::cout << "[Error] because: " << error_reason;
			std::abort();
		}
		return std::move(optional.value());
	}
};

}
