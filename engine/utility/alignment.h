#pragma once

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

}
