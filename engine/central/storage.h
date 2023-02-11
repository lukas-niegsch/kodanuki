#pragma once
#include <any>
#include <map>
#include <set>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <memory>

namespace kodanuki {

/**
 * The entity storage class contains arbitrary amount of data.
 * 
 * Most methods use sparse identifiers that map to positions inside the dense
 * vector. Values are stored using std::any to avoid templates. However, all
 * elements should be of the same type.
 */
class EntityStorage
{
public:
	// Updates the given element or inserts it into the storage.
	void update(int identifier, std::any value);

	// Removes the given element from the storage.
	void remove(int identifier);

	// Returns the reference to the element.
	std::any& get(int identifier);

	// Returns true iff the element is inside the storage.
	bool contains(int identifier) const;

	// Returns the number of dense elements inside the storage.
	int size() const;

	// Returns all sparse identifiers inside the storage.
	std::set<int> identifiers(bool includeWeakBindings = false);

	// Copies the source element to the target element inside the storage.
	void copy(int sourceIdentifier, int targetIdentifier);

	// Moves the source element to the target element inside the storage.
	void move(int sourceIdentifier, int targetIdentifier);

	// Swaps the source element with the target element inside the storage.
	void swap(int sourceIdentifier, int targetIdentifier);

	// Binds the source element to the target element inside the storage.
	void bind(int sourceIdentifier, int targetIdentifier, bool weak = false);

private:
	using EID = int; // entity identifier
	using SID = int; // storage identifier
	using POS = int; // dense position
	std::map<EID, SID> bindings;
	std::set<EID> weakBindings;
	std::map<SID, POS> sparse;
	std::map<POS, SID> sparseInverse;
	std::map<SID, int> sparseCount;
	std::vector<std::any> dense;
};

using Storage = std::unique_ptr<EntityStorage>;
using Mapping = std::unordered_map<std::type_index, Storage>;

/**
 * Returns the storage for the given mapping. Missing type entries will be
 * added automatically.
 */
template <typename T>
static EntityStorage* getStorage(Mapping& mapping)
{
	auto type = std::type_index(typeid(T));
	if (mapping.count(type) == 0) {
		mapping[type] = std::make_unique<EntityStorage>();
	}
	return mapping[type].get();
}

} // namespace kodanuki
