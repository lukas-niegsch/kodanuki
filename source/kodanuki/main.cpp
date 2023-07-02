#include <iostream>
#include "engine/display/window.h"
using namespace kodanuki;

int main()
{
	Window window = Window::create({
		.owner  = std::nullopt,
		.dtype  = Window::Vulkan | Window::OpenGL,
		.title  = "Kodanuki",
		.icon   = "assets/images/tanuki.png",
		.flags  = Window::Resizable | Window::Decorated,
		.shape  = {600, 600},
		.frames = 60
	});

	while (window.tick()) {}
}
