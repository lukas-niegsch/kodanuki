/**
 * Plugin API Structure
 * ====================
 * 
 * The current plugin system is very basic. It is static and only works
 * with functions. We define multiple macros that let users call plugin
 * methods provided they have the name of the plugin.
 *
 * How does it work?
 * -----------------
 * 
 * Plugin functions that require the plugin name can be implemented
 * by different plugins. For example, let's assume we implemented
 * draw_triangle() with vulkan and opengl.
 * 
 * void draw_triangle(int[] vertices, int[] colors);
 * 
 * These functions can be used abstractly using macros:
 * 
 * API("vulkan", draw_triangle)(vertices, colors);
 * API("opengl", draw_triangle)(vertices, colors);
 * 
 * The user can then specify a plugin name for the module. Whenever the
 * system makes an API call, the function from the specified plugin is
 * called. It allows us to write abtract code that can be implemented by
 * different plugins and used simultaneously.
 * 
 * Plugins implement these functions and register them:
 * 
 * void draw_triangle (int[] vertices, int[] colors) {}
 * REGISTER_PLUGIN_METHOD("vulkan", draw_triangle);
 * 
 * It is important that these functions have the same signature and name.
 * The plugin can be statically linked inside the application to register
 * all api functions. Using unimplemented functions or specifying the wrong
 * plugin name will result in runtime errors.
 */
#include "engine/central/plugin.h"
#include <doctest/doctest.h>
using namespace Kodanuki;

int add(int a, int b)
{
	return a + b;
}

std::string append(std::string a, int b)
{
	return a + std::to_string(b);
}

TEST_CASE("plugin tests")
{
	// static code, must be unique plugin/method pair
	REGISTER_PLUGIN_METHOD("test", add);
	REGISTER_PLUGIN_METHOD("test", append);
	REGISTER_PLUGIN_METHOD("next", append);

	// it can be called afterwards
	auto f = API("test", add);
	CHECK(f(3, 5) == 8);
	CHECK(f(4, 9) == 13);
	CHECK(API("test", append)("a", 5) == "a5");

	// asserts that both plugin/method pair is usable
	// doctest does not work well with asserts
	// CHECK_THROWS(API("unknown", append));
};

