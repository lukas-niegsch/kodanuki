#include "engine/central/utility/alignment.h"

namespace kodanuki
{

uint32_t align_modulo(uint32_t value, uint32_t mod)
{
	return (value / mod + (value % mod != 0)) * mod;
}

}
