#pragma once
#include "engine/central/storage.h"
#include <map>
#include <vector>

namespace Kodanuki
{

template <typename ... T>
struct Predicate
{
	static void countIds(std::map<int, int>& counter, Mapping& mapping, bool weak);
	static int size();
};

template <typename ... T>
struct Iterator : public Predicate<T...>
{
	Iterator(Mapping& mapping, std::vector<int> ids, int position = 0);
	Iterator begin();
	Iterator end();
	std::tuple<T&...> operator*();
	void operator++();
	bool operator!=(const Iterator& other);

private:
	Mapping& mapping;
	std::vector<int> ids;
	int position;
};

/**
 * An archetype groups multiple components together for iteration.
 * 
 * The archetype will iterate over entities which contain all components. The
 * "Predicate" components are skipped during the iteration but are still
 * considered in the intersection. The "Iterator" components are the ones
 * that we iterate over.
 * 
 * e.g.
 * Archetype<Iterator<int, float>, Predicate<const char*>>
 * -> consider all entities with int, float, and const char* components
 * -> take the intersection of these entities (the ones with all 3 components)
 * -> iterate over int and float components for the intersecting entities
 * 
 * Iterator must have at least one component. Weak iteration will skip over
 * all components which are weakly binded.
 */
template <typename Iterator, typename Predicate>
struct Archetype
{
	static auto iterate(Mapping& mapping, bool weak = false)
	{
		std::map<int, int> counter;
		Predicate::countIds(counter, mapping, weak);
		Iterator::countIds(counter, mapping, weak);
		int target = Predicate::size() + Iterator::size();
		std::vector<int> intersection;
		for (auto[id, count] : counter) {
			if (count == target) {
				intersection.push_back(id);
			}
		}
		return Iterator(mapping, intersection);
	}
};

}

#include "engine/central/archetype.tpp"
