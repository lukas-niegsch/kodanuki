#pragma once
#include <memory>
#include <functional>

namespace kodanuki
{

/**
 * Shared wrapper around some given type.
 *
 * Each wrapper basically behaves the same way as a shared_ptr but it
 * also convertes to the base type. This just makes it easier to use
 * them in methods without having to dereference it. Also allows the
 * user to specify some custom destroy method.
 */
template <typename T>
struct Wrapper
{
public:
	/**
	 * Creates a new wrapper around the pointer.
	 *
	 * This wrapper now owns the pointer. It will destroy the pointer once
	 * this class is no longer used.
	 *
	 * @param ptr The pointer which will be wrapped.
	 * @param destroy The custom destroy method.
	 */
	Wrapper(T* ptr, std::function<void(T*)> destroy)
	{
		this->state = std::shared_ptr<T>(ptr, destroy);
	}

	/**
	 * Dereferences and returns the underlying type.
	 */
	operator T&()
	{
		return *state;
	}

	/**
	 * Dereferences and returns the underlying type.
	 */
	operator const T&() const
	{
		return *state;
	}

private:
	std::shared_ptr<T> state;
};

}
