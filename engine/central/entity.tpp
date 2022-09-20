#include "engine/central/entity.h"
#include <any>

namespace Kodanuki
{

template <typename T>
void ECS::update(Entity entity, T component)
{
	EntityStorage* storage = getStorage<T>(mapping);
	storage->update(entity.value(), component);
}

template <typename T>
void ECS::remove(Entity entity)
{
	if constexpr (std::is_same<T, Entity>()) {
		for (auto& pair : mapping) {
			pair.second->remove(entity.value());
		}
		return;
	}
	EntityStorage* storage = getStorage<T>(mapping);
	storage->remove(entity.value());	
}

template <typename T>
bool ECS::has(Entity entity)
{
	EntityStorage* storage = getStorage<T>(mapping);
	return storage->contains(entity.value());
}

template <typename T>
T& ECS::get(Entity entity)
{
	EntityStorage* storage = getStorage<T>(mapping);
	return std::any_cast<T&>(storage->get(entity.value()));
}

template <typename T>
void ECS::copy(Entity source, Entity target)
{
	EntityStorage* storage = getStorage<T>(mapping);
	storage->copy(source.value(), target.value());
}

template <typename T>
void ECS::move(Entity source, Entity target)
{
	EntityStorage* storage = getStorage<T>(mapping);
	storage->move(source.value(), target.value());
}

template <typename T>
void ECS::swap(Entity source, Entity target)
{
	EntityStorage* storage = getStorage<T>(mapping);
	storage->swap(source.value(), target.value());
}

template <typename T>
void ECS::bind(Entity source, Entity target)
{
	EntityStorage* storage = getStorage<T>(mapping);
	storage->bind(source.value(), target.value());
}

template <typename Archetype>
auto ECS::iterate()
{
	return Archetype::iterate(mapping);
}

}
