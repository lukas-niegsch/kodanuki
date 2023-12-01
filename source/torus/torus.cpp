#include "source/torus/vkbridge.h"


int main()
{
	VulkanDevice device = create_device();
	VulkanWindow window = create_window(device, 1950, 1200);
	VulkanTarget target = create_target(device, window);
	vkdraw::fn_draw triangle = create_example_triangle(device, target);

	bool running = true;
	sf::WindowBase& native_window = window.window;

	while (running) {
		sf::Event event;
		while (native_window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				running = false;
			} else if (event.type == sf::Event::Resized) {
				window.recreate(device);
			}
		}

		vkdraw::aquire_frame(device, window);
		vkdraw::record_frame(device, window, {triangle});
		vkdraw::submit_frame(device, window);
		vkdraw::render_frame(device, window);
	}

	CHECK_VULKAN(vkDeviceWaitIdle(device));
	return 0;
}
