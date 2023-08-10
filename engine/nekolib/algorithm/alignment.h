#pragma once
#include <cstdint>
#include <concepts>

namespace kodanuki
{

/**
 * Aligns the type T with the given modulus. The modulus must
 * be a power of 2 (gcc specification).
 *
 * Example:
 * sizeof(glm::vec3) == 12, sizeof(align<glm::vec3, 16>) == 16
 */
template <typename T, std::size_t modulus>
struct alignas(modulus) align : T
{
	static_assert(std::copyable<T>, "T must be copyable");
	static_assert(std::movable<T>, "T must be movable");
	using T::T;
	using T::operator=;
	static constexpr std::size_t size = sizeof(align<T, modulus>);
};

/**
 * Returns the smallest integer bigger than the value with zero modulos.
 *
 * @param value The given lower bound for the result.
 * @param mod The given modulos for the alignment.
 * @return The next multiple of mode bigger than value.
 */
inline uint32_t align_modulo(uint32_t value, uint32_t mod)
{
	return (value / mod + (value % mod != 0)) * mod;
}

}
