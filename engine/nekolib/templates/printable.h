#pragma once
#include <ostream>

namespace kodanuki
{

template <typename T>
concept printable = requires (T v, std::ostream& os) { os << v; };

}
