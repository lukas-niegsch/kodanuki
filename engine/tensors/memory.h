#pragma once
#include <vector>

namespace kodanuki
{

class MemoryView
{
public:
	MemoryView(float&);

public:
	float& as_float();

private:
	float& value;
};

class Memory
{
public:
	enum DataType
	{
		FLOAT
	};

	std::size_t get_size() const;
	void resize(std::size_t size);
	MemoryView make_view(std::size_t index);

private:
	std::vector<float> values;
};

}
