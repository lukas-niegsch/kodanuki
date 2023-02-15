#pragma once
#include <memory>

namespace kodanuki
{

struct OperatorBuilder
{

};

class Operator
{
public:
	Operator(OperatorBuilder builder);

private:
	std::shared_ptr<struct OperatorState> state;
};

}
