#include "source/splash/pipelines.h"
#include "source/splash/user_interface.h"
#include "source/splash/shader_bridge.h"
#include "engine/vulkan/debug.h"
#include "engine/vulkan/device.h"
#include "engine/vulkan/window.h"
#include "engine/vulkan/memory.h"
#include "engine/vulkan/target.h"
#include "engine/vulkan/renderer.h"
#include "source/splash/model.h"
#include "source/splash/scene.h"
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
		.width = 600,
		.height = 600,
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
		.queue_priorities = {1.0f, 0.5f, 1.0f}
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
	VulkanPipeline update_fluid_pressure = create_update_fluid_pipeline_pressure(device);
	VulkanPipeline update_fluid_simulate = create_update_fluid_pipeline_simulate(device);

	ShaderBridge bridge = {{
		.device = device,
		.frame_count = target.get_frame_count(),
		.model = load_obj_model("assets/models/sphere.obj"),
		.scene = load_csv_scene("assets/models/debug.csv"),
		.render_pipeline = render_fluid,
		.update_pipeline_pressure = update_fluid_pressure,
		.update_pipeline_simulate = update_fluid_simulate
	}};

	uint32_t index_count = bridge.get_index_count();
	uint32_t instance_count = bridge.get_instance_count();

	Config config;
	glm::vec3 player_position = {0.0f, 20.0f, 0.0f};
	glm::vec3 player_rotation = {0.0f, 0.0f, 0.0f};

	while (interface.tick())
	{
		float dts = window.get_delta_time_seconds();
		uint32_t frame = renderer.aquire_frame();
		handle_user_inputs(config, dts, frame, window, target, bridge, player_position, player_rotation);
		show_config(config, dts);

		auto buffer = bridge.get_buffer_ud(frame);
		buffer[0].delta_time = dts;
		buffer[0].kernel_size = 1.2;
		buffer[0].stiffness = 0.0;
		buffer[0].rho_0 = 1.0;
		buffer[0].gravity = 10;
		buffer[0].viscosity = 0.2;
		buffer[0].particle_count = instance_count;
		buffer.slow_push();

		renderer.call_command([&](VkCommandBuffer buffer) {
			vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_COMPUTE, update_fluid_pressure);
			bridge.bind_update_pressure_resources(buffer, frame);
			vkCmdDispatch(buffer, (instance_count / 128) + 1, 1, 1);
		}, 2);

		renderer.call_command([&](VkCommandBuffer buffer) {
			vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_COMPUTE, update_fluid_simulate);
			bridge.bind_update_simulate_resources(buffer, frame);
			vkCmdDispatch(buffer, (instance_count / 128) + 1, 1, 1);
		}, 2);

		renderer.draw_command([&](VkCommandBuffer buffer) {
			vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, render_fluid);
			bridge.bind_render_resources(buffer, frame);
			vkCmdDrawIndexed(buffer, index_count, instance_count, 0, 0, 0);
			interface.draw(buffer);
		});

		renderer.submit_frame();
		renderer.render_frame();
	}

	return 0;
}
