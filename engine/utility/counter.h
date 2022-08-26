#pragma once
#include <cstdint>

namespace Kodanuki
{

/**
 * Counts how many times this function was called.
 * 
 * This function is completely thread-safe and can be used for unique
 * identifiers. There are also 2^64 values which makes it very unlikely
 * that this function will ever overflow.
 * 
 * @return uint64_t The number of times the function was called.
 */
uint64_t count();

}
