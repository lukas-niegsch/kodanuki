#include "engine/tensors/memory.h"

namespace kodanuki
{

float& MemoryView::as_float()
{
	static float a = 0;
	return a;
}

}
