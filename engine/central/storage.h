#pragma once
#include "engine/central/utility/counter.h"
#include "engine/nekolib/container/dense_map.h"
#include <algorithm>
#include <any>
#include <cassert>
#include <cstdint>
#include <functional>
#include <set>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace kodanuki
{

/**
 * The entity storage is a unordered sparse-dense map.
 *
 * It holds (key, value) pairs like a unordered map, but stores the
 * values as a contiguous vector. Multiple keys for the same value are
 * allowed.
 *
 * We map the key into a sparse map containing the position inside
 * the dense vector. By layering two maps together we achieve the
 * same complexity as unordered_map for most operations. But with an
 * additional overhead for the pointer redirections.
 *
 * Using different keys for the same value works with the bind()
 * method. The value is removed once every key for that value is
 * removed. Updating that values updates it for all keys since it
 * points to the same memory.
 *
 * Each key should be unique, updating with the same key removes the
 * old value and inserts the new one.
 */
template <typename T>
class EntityStorage
{
public:
	/**
	 * Updates the given element or inserts it.
	 *
	 * @param key The key of the updated value.
	 * @param value The new value for the key.
	 */
	void update(uint64_t key, T value)
	{
		if (contains(key)) {
			(*this)[key] = value;
			return;
		}
		insert(key, value);
	}

	/**
	 * Removes the given element from the storage.
	 *
	 * @param key The key of the removed value.
	 */
	void remove(uint64_t key)
	{
		if (!contains(key)) {
			return;
		}
		uint64_t sid = bindings[key];
		bindings.erase(key);
		bindings_count[sid]--;
		if (bindings_count[sid] > 0) {
			return;
		}
		bindings_count.erase(sid);
		dense.remove(sid);
	}

	/**
	 * Binds the source element to the target element inside the storage.
	 *
	 * @param source_key The source key that should be binded.
	 * @param target_key The target key to which to bind.
	 */
	void bind(uint64_t source_key, uint64_t target_key)
	{
		if (!contains(target_key)) {
			return;
		}
		if (contains(source_key)) {
			remove(source_key);
		}
		int sid = bindings[target_key];
		bindings[source_key] = sid;
		bindings_count[sid]++;
	}
	
	/**
	 * Returns the reference to the value.
	 *
	 * @param key The key that points to the value.
	 * @return The reference to the value.
	 */
	T& operator[](uint64_t key)
	{
		assert(contains(key));
		return dense[bindings[key]];
	}

	/**
	 * Returns true iff the value is inside the storage.
	 *
	 * @param key The key that points to the value.
	 * @return Is the value inside this storage?
	 */
	bool contains(uint64_t key) const
	{
		return bindings.count(key);
	}

	/**
	 * Returns the number of keys inside the storage.
	 *
	 * @return The number of keys inside the storage.
	 */
	uint64_t size() const
	{
		return bindings.size();
	}

	/**
	 * Returns all keys inside the storage.
	 *
	 * @return The keys inside the storage.
	 */
	std::set<uint64_t> keys() const
	{
		std::set<uint64_t> result;
		for (auto[key, _] : bindings) {
			result.insert(key);
		}
		return result;
	}

private:
	void insert(uint64_t key, T value)
	{
		uint64_t sid = count();
		bindings[key] = sid;
		bindings_count[sid] = 1;
		dense.update(sid, value);
	}

private:
	std::unordered_map<uint64_t, uint64_t> bindings;
	std::unordered_map<uint64_t, uint64_t> bindings_count;
	DenseMap<uint64_t, T> dense;
};

/**
 * The entity mapping stores multiple entity storages.
 */
class EntityMapping
{
public:
	// The type of the underlying mapping.
	using Mapping = std::unordered_map<std::type_index, std::any>;

	// The type of the map storing callable versions of the remove method.
	using Remover = std::unordered_map<std::type_index, std::function<void(uint64_t)>>;

	// Returns the typed version of this class from the mapping.
	template <typename T>
	EntityStorage<T>& get()
	{
		auto type = std::type_index(typeid(T));
		if (mapping.count(type) == 0) {
			mapping[type] = EntityStorage<T>();
			remover[type] = [this, type](uint64_t id){
				auto& storage = std::any_cast<EntityStorage<T>&>(this->mapping[type]);
				storage.remove(id);
			};
		}
		return std::any_cast<EntityStorage<T>&>(mapping[type]);
	}

	inline void remove(uint64_t id)
	{
		for (auto[_, fn_remove] : remover) {
			fn_remove(id);
		}
	}

private:
	static inline Mapping mapping;
	static inline Remover remover;
};

}
