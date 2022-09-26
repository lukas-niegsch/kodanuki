#pragma once
#include <functional>
#include <string>
#include <unordered_map>
#include <cassert>

namespace Kodanuki
{

/**
 * Registers a given plugin method for the plugin with the given name.
 */
#define REGISTER_PLUGIN_METHOD(name, method) \
	PluginRegistry<decltype(&method)>::set(name, STRINGIFY(method), &method)

/**
 * Calls the API method from a plugin with the given name.
 */
#define API(name, method) \
	PluginRegistry<decltype(&method)>::get(name, STRINGIFY(method))

/**
 * Helper macro that turns a variable into a string.
 */
#define STRINGIFY(x) #x

/**
 * The plugin registry saves functions with the same signature. Should only
 * be used with the above macros.
 */
template <typename Function>
struct PluginRegistry
{
	// Renames some types to avoid long lines.
	using FMAP = std::unordered_map<std::string, Function>;
	using FunctionMapping = std::unordered_map<std::string, FMAP>;

	// Sets the given function inside the function mapping.
	static void set(std::string plugin, std::string name, Function method)
	{
		auto& mapping = functions();
		mapping[plugin][name] = method;
	}

	// Gets the given function inside the function mapping.
	static Function get(std::string plugin, std::string name)
	{
		auto& mapping = functions();
		assert(mapping.count(plugin) > 0);
		assert(mapping[plugin].count(name) > 0);
		return mapping[plugin][name];
	}

	// Meyer's singleton to prevent Static Initialization Order Fiasco (SIOF).
	static FunctionMapping& functions() {
		static FunctionMapping functions;
		return functions;
	}
};

}

