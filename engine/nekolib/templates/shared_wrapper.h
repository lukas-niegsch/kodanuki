#pragma once
#include <cassert>
#include <functional>
#include <memory>


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
struct shared_wrapper_t
{
public:
	/**
	 * Default constructor so this class plays nicely with std::shared_ptr.
	 */
	shared_wrapper_t() = default;

	/**
	 * Creates a new wrapper around the pointer.
	 *
	 * This wrapper now owns the pointer. It will destroy the pointer once
	 * this class is no longer used.
	 *
	 * @param ptr The pointer which will be wrapped.
	 * @param destroy The custom destroy method.
	 */
	shared_wrapper_t(T* ptr, std::function<void(T*)> destroy)
	{
		this->handle = std::shared_ptr<T>(ptr, destroy);
	}

	/**
	 * Dereferences and returns the underlying type.
	 */
	operator T() const
	{
		if (handle) {
			return *handle;
		}
		return {};
	}

	/**
	 * Dereferences and returns the underlying type.
	 */
	operator T&()
	{
		assert(handle);
		return *handle;
	}

	/**
	 * Returns the underlying type.
	 */
	operator T*() const
	{
		return handle.get();
	}

private:
	std::shared_ptr<T> handle;
};

}
