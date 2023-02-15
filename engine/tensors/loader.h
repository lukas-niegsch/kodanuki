#pragma once
#include "engine/tensors/variable.h"
#include <memory>
#include <vector>

namespace kodanuki
{

struct DataLoaderBuilder
{

};

class DataLoader
{
public:
	DataLoader(DataLoaderBuilder builder);

public:
	static DataLoader from_full(Variable value);
	static DataLoader from_diag(Variable value);
	static DataLoader from_diag(std::vector<Variable> values);

private:
	std::shared_ptr<struct DataLoaderState> state;
};

}
