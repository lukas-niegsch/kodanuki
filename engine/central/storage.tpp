#include "engine/central/storage.h"
#include "engine/utility/counter.h"
#include <algorithm>
#include <cassert>

namespace Kodanuki {

template <typename T>
void EntityStorage::remove(int identifier)
{
	if (!contains(identifier))
	{
		return;
	}

	int storageIdentifier = bindings[identifier];
	bindings.erase(identifier);
	weakBindings.erase(identifier);
	sparseCount[storageIdentifier]--;

	if (sparseCount[storageIdentifier] > 0)
	{
		return;
	}

	int position = sparse[storageIdentifier];
	int lastPosition = dense.size() - 1;

	dense[position] = std::move(dense.back());
	dense.pop_back();

	std::swap(sparseInverse[position], sparseInverse[lastPosition]);
	sparse[sparseInverse[position]] = position;
	sparse.erase(sparseInverse[lastPosition]);
	sparseInverse.erase(lastPosition);
	sparseCount.erase(storageIdentifier);
}

template <typename T>
bool EntityStorage::contains(int identifier) const
{
	return bindings.count(identifier);
}

template <typename T>
int EntityStorage::size() const
{
	return bindings.size();
}

template <typename T>
std::set<int> EntityStorage::identifiers(bool weak)
{
	std::set<int> result;

	for (auto it = bindings.begin(); it != bindings.end(); it++)
	{
		if (!weak && weakBindings.count(it->first))
		{
			continue;
		}
	
		result.insert(it->first);
	}

	return result;
}

template <typename T>
void EntityStorage::copy(int source, int target)
{
	if (!contains(source))
	{
		return;
	}
	
	update(target, get(source));
}

template <typename T>
void EntityStorage::move(int source, int target)
{
	if (!contains(source))
	{
		return;
	}
	
	bindings[target] = bindings[source];
	bindings.erase(source);
}

template <typename T>
void EntityStorage::swap(int source, int target)
{
	if (!contains(source))
	{
		move(target, source);
		return;
	}

	if (!contains(target))
	{
		move(source, target);
		return;
	}

	std::swap(bindings[source], bindings[target]);
}

template <typename T>
void EntityStorage::bind(int source, int target, bool weak)
{
	if (!contains(target))
	{
		return;
	}

	if (contains(source))
	{
		remove(source);
	}

	int storageIdentifier = bindings[target];
	bindings[source] = storageIdentifier;
	sparseCount[storageIdentifier]++;

	if (weak)
	{
		weakBindings.insert(source);
	}
}

template <typename T>
std::any& EntityStorage::get(int identifier)
{
	assert(contains(identifier));
	int position = sparse[bindings[identifier]];
	return dense[position];
}

template <typename T>
void EntityStorage::update(int identifier, std::any value)
{
	if (contains(identifier))
	{
		get(identifier) = value;
		return;
	}

	dense.push_back(value);
	int storageIdentifier = count();
	int lastPosition = dense.size() - 1;
	bindings[identifier] = storageIdentifier;
	sparse[storageIdentifier] = lastPosition;
	sparseInverse[lastPosition] = storageIdentifier;
	sparseCount[storageIdentifier] = 1;
}

} // namespace Kodanuki