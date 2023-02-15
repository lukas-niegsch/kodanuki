#pragma once
#include <variant>
#include <cstdint>
#include <complex>

namespace kodanuki
{

enum class VariableType
{
	BOOL,
	BYTE,
	INT32,
	UINT32,
	INT64,
	UINT64,
	FLOAT,
	DOUBLE,
	COMPLEX
};

typedef std::variant<
	bool,
	char,
	std::int32_t,
	std::uint32_t,
	std::int64_t,
	std::uint64_t,
	float,
	double,
	std::complex<double>
> Variable;

}
