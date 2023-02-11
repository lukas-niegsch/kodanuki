#pragma once
#include "engine/central/entity.h"
#include <memory>

namespace kodanuki
{

/**
 * Copyable wrapper combine two patterns: copyable crtp and pimpl idiom.
 * 
 * All subclasses can be copied by value freely. Changes to any copy will
 * affect all others (similar pointer). Additionally the shared_destructor()
 * method will be called once all copies are no longer used. However instances
 * can never be nullptr and are always valid. Additionally, subclasses can
 * access the "impl" entity.
 *
 * Subclasses are required to provide a shared_destructor() function.
 */
template <typename CRTP>
struct Copyable
{
public:
	Copyable()
	{
		auto deleter = [this](Entity* pimpl) { shared_destructor(pimpl); };
		pimpl = std::shared_ptr<Entity>(new Entity, deleter);
		impl = *pimpl = ECS::create();
	}

public:
	bool operator==(const Copyable& other)
	{
		return *pimpl == *other.pimpl;
	}

private:
	void shared_destructor(Entity* pimpl)
	{
		static_cast<CRTP*>(this)->shared_destructor();
		ECS::remove<Entity>(impl);
		delete pimpl;
	}

protected:
	Entity impl; // To avoid using *pimpl all the time.

private:
	std::shared_ptr<Entity> pimpl;
};

}
