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

std::size_t Memory::get_size() const
{
	return this->values.size();
}

void Memory::resize(std::size_t size)
{
	this->values.resize(size);
}

MemoryView Memory::make_view(std::size_t index)
{
	float& value = values[index];
	return MemoryView(value);
}

}
