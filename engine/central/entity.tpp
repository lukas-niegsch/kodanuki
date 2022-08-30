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
	EntityStorage* storage = getStorage<T>(mapping);
	storage->remove(entity.value());	
}

template <typename T>
void ECS::remove()
{
	auto type = std::type_index(typeid(T));
	mapping.erase(type);
}

template <typename T>
bool ECS::has(Entity entity)
{
	EntityStorage* storage = getStorage<T>(mapping);
	return storage->contains(entity.value());
}

template <typename T>
bool ECS::has()
{
	EntityStorage* storage = getStorage<T>(mapping);
	return storage->size() > 0;
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

template <typename A, typename B, typename ... T>
void ECS::copy(Entity source, Entity target)
{
	ECS::copy<A>(source, target);
	ECS::copy<B, T...>(source, target);
}

template <typename T>
void ECS::move(Entity source, Entity target)
{
	EntityStorage* storage = getStorage<T>(mapping);
	storage->move(source.value(), target.value());
}

template <typename A, typename B, typename ... T>
void ECS::move(Entity source, Entity target)
{
	ECS::move<A>(source, target);
	ECS::move<B, T...>(source, target);
}

template <typename T>
void ECS::swap(Entity source, Entity target)
{
	EntityStorage* storage = getStorage<T>(mapping);
	storage->swap(source.value(), target.value());
}

template <typename A, typename B, typename ... T>
void ECS::swap(Entity source, Entity target)
{
	ECS::swap<A>(source, target);
	ECS::swap<B, T...>(source, target);
}

template <typename T>
void ECS::bind(Entity source, Entity target, bool weak)
{
	EntityStorage* storage = getStorage<T>(mapping);
	storage->bind(source.value(), target.value(), weak);
}

template <typename A, typename B, typename ... T>
void ECS::bind(Entity source, Entity target, bool weak)
{
	ECS::bind<A>(source, target, weak);
	ECS::bind<B, T...>(source, target, weak);
}

template <typename Archetype>
auto ECS::iterate(bool weak)
{
	return Archetype::iterate(mapping, weak);
}

}
