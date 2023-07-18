Testing Playground
==================

Goal for the display system, maybe different names and parameters ...

.. code-block:: c++

	void create_window()
	{
		window = Window::create({
			.owner  = root,
			.theme = ...,
			.flags  = Window::Default,
			.shape  = {600, 600},
			.title  = "Kodanuki",
			.frames = 60
		});

		Widget w1 = Border::create({
			.owner = root,
			.size_left  = 2,
			.size_right = 2,
			.size_top   = 1,
			.size_bot   = 1
		});

		Widget w2 = Linear::create({
			.owner = w1,
			.flags = Linear::LeftToRight,
			.separator_size = 1
		});

		Widget w3 = LinearItem::create({
			.owner = w2,
			.weight = 1,
			.simple_border = 1
		});

		Widget w4 = LinearItem::create({
			.owner = w2,
			.weight = 1,
			.simple_border = 1
		});

		VulkanViewport::create({
			.owner = w3,
			.device = ...,
			...
		});

		OpenglViewport::create({
			.owner = w4,
			...
		});
	}
