#include "engine/utility/counter.h"
#include <atomic>

namespace Kodanuki
{

uint64_t count()
{
	static std::atomic<uint64_t> counter;
	return counter++;
}

}
