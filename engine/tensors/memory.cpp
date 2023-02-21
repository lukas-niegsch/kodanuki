#include "engine/tensors/memory.h"

namespace kodanuki
{

MemoryView::MemoryView(float& value) : value(value)
{

}

float& MemoryView::as_float()
{
	return value;
}

MemoryView Memory::make_view()
{
	return MemoryView(value);
}

}
