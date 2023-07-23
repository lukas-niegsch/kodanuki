#pragma once
#include <iterator>

namespace kodanuki
{

template <typename T>
concept iterable = requires (T v) { std::begin(v); std::end(v); };

}
