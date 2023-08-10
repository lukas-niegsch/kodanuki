#pragma once
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace kodanuki
{

/**
 * Implementation of an unordered key value map where values are stored
 * inside contiguous memory. The keys are stored in a sparse map pointing
 * to positions in a dense vector. Copying the elements inside the vector
 * after each remove is avoided using swap-back removes. First swap the
 * element with the back element and then pop_back. To implement this in
 * O(1) we also keep the reverse map of the sparse map. The order of the
 * elements in the dense vector is effectively random. We implement parts
 * of the interface similar to the STL.
 */
template <typename K, typename V>
class DenseMap
{
public: // Modifiers
	/**
	 * Clears the content of this container.
	 */
	void clear() noexcept
	{
		sparse_forward.clear();
		sparse_inverse.clear();
		dense.clear();
	}

	/**
	 * Updates the given element or inserts it.
	 *
	 * @param key The key of the element to insert.
	 * @param value The new value of the element.
	 */
	void update(const K& key, V& value) noexcept
	{
		if (contains(key)) {
			at(key) = value;
			return;
		}
		uint64_t pos = dense.size();
		dense.push_back(value);
		sparse_forward[key] = pos;
		sparse_inverse[pos] = key;
	}

	/**
	 * Removes the given element if found.
	 *
	 * @param key The key of the element to find.
	 */
	void remove(const K& key) noexcept
	{
		if (!contains(key)) {
			return;
		}
		uint64_t key_pos = sparse_forward[key];
		uint64_t end_pos = dense.size() - 1;
		dense[key_pos] = std::move(dense.back());
		dense.pop_back();
		sparse_forward[sparse_inverse[end_pos]] = key_pos;
		sparse_forward.erase(key);
		sparse_inverse[key_pos] = sparse_inverse[end_pos]; 
		sparse_inverse.erase(end_pos);
	}

public: // Element access
	/**
	 * Returns the mapped element for the given key.
	 *
	 * @param key The key of the element to find.
	 * @return The reference to the requested element.
	 * @throws when the element was not found. 
	 */
	V& at(const K& key)
	{
		return dense[sparse_forward[key]];
	}

	/**
	 * Returns the mapped element for the given key.
	 *
	 * @param key The key of the element to find.
	 * @return The reference to the requested element.
	 * @throws when the element was not found. 
	 */
	const V& at(const K& key) const
	{
		return dense[sparse_forward[key]];
	}

	/**
	 * Returns the mapped element for the given key.
	 *
	 * @param key The key of the element to find.
	 * @return The reference to the requested element.
	 * @throws when the element was not found. 
	 */
	V& operator[](const K& key)
	{
		return at(key);
	}

	/**
	 * Returns the mapped element for the given key.
	 *
	 * @param key The key of the element to find.
	 * @return The reference to the requested element.
	 * @throws when the element was not found. 
	 */
	const V& operator[](const K& key) const
	{
		return at(key);
	}

	/**
	 * Returns the number of element with the given key.
	 *
	 * @param key The key of the element to find.
	 * @return One if the key is inside the the container, zero otherwise.
	 */
	std::size_t count(const K& key) const
	{
		return sparse_forward.count(key);
	}

	/**
	 * @param key The key of the element to find.
	 * @return Is the element with the given key inside the sparse map?
	 */
	bool contains(const K& key) const
	{
		return sparse_forward.contains(key);
	}

	/**
	 * @param key The key of the element to find.
	 * @return The iterator to the element or to the end() if not found.
	 */
	std::vector<V>::iterator find(const K& key)
	{
		auto it = sparse_forward.find(key);
		auto end = sparse_forward.end();
		return it == end ? dense.begin() + *it : dense.end();
	}

	/**
	 * @param key The key of the element to find.
	 * @return The iterator to the element or to the end() if not found.
	 */
	std::vector<V>::const_iterator find(const K& key) const
	{
		auto it = sparse_forward.find(key);
		auto end = sparse_forward.end();
		return it == end ? dense.begin() + *it : dense.end();
	}

	/**
	 * @return The reference to the first element inside the dense vector.
	 */
	constexpr V& front()
	{
		return dense.front();
	}

	/**
	 * @return The reference to the first element inside the dense vector.
	 */
	constexpr const V& front() const
	{
		return dense.front();
	}

	/**
	 * @return The reference to the last element inside the dense vector.
	 */
	constexpr V& back()
	{
		return dense.back();
	}

	/**
	 * @return The reference to the last element inside the dense vector.
	 */
	constexpr const V& back() const
	{
		return dense.back();
	}

	/**
	 * @return The pointer to the data inside the dense vector.
	 */
	constexpr V* data() noexcept
	{
		return dense.data();
	}

	/**
	 * @return The pointer to the data inside the dense vector.
	 */
	constexpr const V* data() const noexcept
	{
		return dense.data();
	}

public: // Capacity
	/**
	 * Checks whether the container is empty.
	 *
	 * @return Is the sparse map empty?
	 */
	[[nodiscard]] bool empty() const noexcept
	{
		return sparse_forward.empty();
	}

	/**
	 * Returns the number of elements.
	 *
	 * @return The number of elements inside the sparse map.
	 */
	std::size_t size() const noexcept
	{
		return sparse_forward.empty();
	}

	/**
	 * Returns the maximum number of elements the container can hold.
	 *
	 * @return The maximum number of elements in the underlying containers.
	 */
	std::size_t max_size() const noexcept
	{
		return std::min(sparse_forward.max_size(), dense.max_size());
	}

public: // Iterators
	/**
	 * @return The iterator to the beginning of the dense vector.
	 */
	constexpr std::vector<V>::iterator begin() noexcept
	{
		return dense.begin();
	}

	/**
	 * @return The iterator to the beginning of the dense vector.
	 */
	constexpr std::vector<V>::const_iterator begin() const noexcept
	{
		return dense.begin();
	}

	/**
	 * @return The iterator to the beginning of the dense vector.
	 */
	constexpr std::vector<V>::const_iterator cbegin() const noexcept
	{
		return dense.cbegin();
	}

	/**
	 * @return The iterator to the end of the dense vector.
	 */
	constexpr std::vector<V>::iterator end() noexcept
	{
		return dense.end();
	}

	/**
	 * @return The iterator to the end of the dense vector.
	 */
	constexpr std::vector<V>::const_iterator end() const noexcept
	{
		return dense.end();
	}

	/**
	 * @return The iterator to the end of the dense vector.
	 */
	constexpr std::vector<V>::const_iterator cend() const noexcept
	{
		return dense.cend();
	}

	/**
	 * @return The reverse iterator to the beginning of the dense vector.
	 */
	constexpr std::vector<V>::reverse_iterator rbegin() noexcept
	{
		return dense.rbegin();
	}

	/**
	 * @return The reverse iterator to the beginning of the dense vector.
	 */
	constexpr std::vector<V>::const_reverse_iterator rbegin() const noexcept
	{
		return dense.rbegin();
	}

	/**
	 * @return The reverse iterator to the beginning of the dense vector.
	 */
	constexpr std::vector<V>::const_reverse_iterator crbegin() const noexcept
	{
		return dense.crbegin();
	}

	/**
	 * @return The reverse iterator to the end of the dense vector.
	 */
	constexpr std::vector<V>::reverse_iterator rend() noexcept
	{
		return dense.rend();
	}

	/**
	 * @return The reverse iterator to the end of the dense vector.
	 */
	constexpr std::vector<V>::const_reverse_iterator rend() const noexcept
	{
		return dense.rend();
	}

	/**
	 * @return The reverse iterator to the end of the dense vector.
	 */
	constexpr std::vector<V>::const_reverse_iterator crend() const noexcept
	{
		return dense.crend();
	}

private:
	std::unordered_map<K, uint64_t> sparse_forward;
	std::unordered_map<uint64_t, K> sparse_inverse;
	std::vector<V> dense;
};

}
