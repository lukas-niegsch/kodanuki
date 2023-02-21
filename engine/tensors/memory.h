#pragma once

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

	MemoryView make_view();

private:
	float value;
};

}
