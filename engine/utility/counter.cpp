#include "engine/utility/counter.h"
#include <atomic>

namespace kodanuki
{

uint64_t count()
{
	static std::atomic<uint64_t> counter;
	return counter++;
}

}
