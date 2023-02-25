#pragma once

namespace kodanuki
{

// Basic information about each operator that lets each implementation know
// how to execute some operation. Does not define any actual logic for these
// operations.
class Operator
{

};

// CRTP that allows the tensor to inherit the given operators
// The name is of the class is choosen so we have the following syntax:
// Tensor c = op::add(a, b);
template <typename Tensor>
struct op
{
	static Tensor add(const Tensor& a, const Tensor& b);
	// Create new AddOperation, call CRTP::execute(), provide interface
};

}
