#include "engine/central/storage.h"
#include <memory>
#include <typeindex>
#include <unordered_map>

namespace Kodanuki
{

using Counter = std::map<int, int>;

template <typename ... T>
void Predicate<T...>::countIds(std::map<int, int>& counter, Mapping& mapping, bool weak)
{
	(void) weak;
	std::set<int> ids[] = { getStorage<T>(mapping)->identifiers(weak) ...};
	for (int i = 0; i < size(); i++) {
		for (int id : ids[i]) {
			counter[id]++;
		}
	}
}

template <typename ... T>
int Predicate<T...>::size()
{
	return sizeof...(T);
}

template <typename ... T>
Iterator<T...>::Iterator(Mapping& mapping, std::vector<int> ids, int position)
	: mapping(mapping), ids(ids), position(position) {}
	
template <typename ... T>
Iterator<T...> Iterator<T...>::begin()
{
	return Iterator(mapping, ids, 0);
}

template <typename ... T>
Iterator<T...> Iterator<T...>::end()
{
	return Iterator(mapping, ids, ids.size());
}

template <typename ... T>
std::tuple<T&...> Iterator<T...>::operator*()
{
	int id = ids[position];
	return std::tie(std::any_cast<T&>(getStorage<T>(mapping)->get(id))...);
}

template <typename ... T>
void Iterator<T...>::operator++()
{
	position++;
}

template <typename ... T>
bool Iterator<T...>::operator!=(const Iterator& other)
{
	return position != other.position; 
}

}