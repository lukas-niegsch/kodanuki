#include "source/splash/pipelines.h"
#include "source/splash/user_interface.h"
#include "source/splash/shader_bridge.h"
#include "engine/vulkan/debug.h"
#include "engine/vulkan/device.h"
#include "engine/vulkan/window.h"
#include "engine/vulkan/target.h"
#include "engine/vulkan/renderer.h"
#include "source/splash/model.h"
#include "source/splash/scene.h"
#include "source/splash/culling.h"
#include "engine/utility/alignment.h"
#include "extern/imgui/imgui.h"
#include "refactor_later.h"
using namespace kodanuki;

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

int main()
{
	VulkanWindow window = {{
		.title = "Splash",
		.width = 800,
		.height = 800,
		.resizeable = false,
		.fullscreen = false,
		.frames = 60
	}};

	VulkanDevice device = {{
		.instance_layers = {"VK_LAYER_KHRONOS_validation"},
		.instance_extensions = window.required_instance_extensions(),
		.device_extensions = {"VK_KHR_swapchain"},
		.gpu_score = &score_physical_device,
		.queue_score = &score_queue_family,
		.queue_priorities = {0.2f, 0.2f, 1.0f}
	}};

	VulkanTarget target = {{
		.device = device,
		.window = window,
		.create_renderpass = &create_simple_renderpass,
		.surface_format = {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
		.depth_image_format = VK_FORMAT_D24_UNORM_S8_UINT,
		.present_mode = VK_PRESENT_MODE_IMMEDIATE_KHR,
		.frame_count = 3,
	}};

	VulkanRenderer renderer = {{
		.device = device,
		.target = target,
		.clear_color = {0.52941f, 0.80784f, 0.92157f, 1.0f}
	}};

	UserInterface interface = {{
		.device = device,
		.target = target,
		.window = window,
		.fontfile = "/usr/local/share/fonts/c/ComicMono.ttf",
		.fontsize = 18.0f
	}};

	VulkanPipeline render_fluid = create_render_fluid_pipeline(device, target);
	ShaderBridge bridge = {{
		.device = device,
		.frame_count = target.get_frame_count(),
		.model = load_obj_model("assets/models/icosahedron.obj"),
		.render_pipeline = render_fluid
	}};

	Simulation simulation(device);
	simulation.load_scene(load_csv_scene("assets/models/debug.csv"));

	uint32_t index_count = bridge.get_index_count();
	uint32_t instance_count = simulation.get_particle_count();

	Config config;
	glm::vec3 player_position = {0.0f, 20.0f, 0.0f};
	glm::vec3 player_rotation = {0.0f, 0.0f, 0.0f};

	while (interface.tick())
	{
		float dts = window.get_delta_time_seconds();
		uint32_t frame = renderer.aquire_frame();
		handle_user_inputs(config, dts, frame, window, target, bridge, player_position, player_rotation);
		
		show_config(config, dts);
		simulation.tick_fluids(dts);

		VulkanTensor position = simulation.get_position();
		VulkanTensor mvp = bridge.get_mvp(frame);
		VulkanTensor visibles = cull_invisible_spheres(position, mvp);
		config.compute_particle_count = position.numel() / 3;
		config.visible_particle_count = visibles.numel() / 3;

		renderer.draw_command([&](VkCommandBuffer buffer) {
			vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, render_fluid);
			bridge.bind_render_resources(buffer, frame, visibles);
			vkCmdDrawIndexed(buffer, index_count, instance_count, 0, 0, 0);
			interface.draw(buffer);
		});

		renderer.submit_frame();
		renderer.render_frame();
	}

	return 0;
}
