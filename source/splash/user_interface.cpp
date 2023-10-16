#include "source/splash/user_interface.h"
#include "source/splash/user_interface_backend.h"
#include "engine/vulkan/debug.h"
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace kodanuki
{

UserInterface::~UserInterface()
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

UserInterface::UserInterface(UserInterfaceBuilder builder)
: device(builder.device)
, window(builder.window)
, target(builder.target)
{
	window.set_cursor_movement_callback([&](float xoffset, float yoffset) {
		if (ImGui::GetIO().WantCaptureMouse) {
			return;
		}
		if (!window.is_mouse_button_pressed(GLFW_MOUSE_BUTTON_LEFT)) {
			return;
		}
		camera.process_mouse_movement(xoffset, yoffset, true);
	});
	
	window.set_cursor_scroll_callback([&](float yoffset) {
		camera.process_mouse_scroll(yoffset);
	});

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF(builder.fontfile.c_str(), builder.fontsize);

	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForVulkan(builder.window, true);

	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = device.get_instance();
	init_info.PhysicalDevice = device.get_physical_device();
	init_info.Device = device;
	init_info.QueueFamily = device.get_queue_family();
	init_info.Queue = device.get_queue(0);
	init_info.DescriptorPool = device.get_descriptor_pool();
	init_info.MinImageCount = 2;
	init_info.ImageCount = target.get_frame_count();
	ImGui_ImplVulkan_Init(&init_info, target.get_renderpass());

	device.execute([&](VkCommandBuffer buffer) {
		ImGui_ImplVulkan_CreateFontsTexture(buffer);
	});
	ImGui_ImplVulkan_DestroyFontUploadObjects();
}

bool UserInterface::tick()
{
	if (!window.tick()) {
		return false;
	}

	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	return true;
}

void UserInterface::draw(VkCommandBuffer buffer)
{
	ImGui::Render();
	ImDrawData* draw_data = ImGui::GetDrawData();
	ImGui_ImplVulkan_RenderDrawData(draw_data, buffer);
}

void UserInterface::handle_input(ShaderBridge& bridge, uint32_t frame, float delta_time_seconds)
{
	camera.MovementSpeed = move_speed;
	camera.MouseSensitivity = look_speed / 200;

	if (window.is_key_pressed(GLFW_KEY_M)) {
		is_demo_open = true;
	}
	if (window.is_key_pressed(GLFW_KEY_N)) {
		is_demo_open = false;
	}
	if (window.is_key_pressed(GLFW_KEY_C)) {
		is_menu_open = true;
	}
	if (window.is_key_pressed(GLFW_KEY_V)) {
		is_menu_open = false;
	}
	if (window.is_key_pressed(GLFW_KEY_A)) {
		camera.process_keyboard(Camera::LEFT, delta_time_seconds);
	}
	if (window.is_key_pressed(GLFW_KEY_S)) {
		camera.process_keyboard(Camera::BACKWARD, delta_time_seconds);
	}
	if (window.is_key_pressed(GLFW_KEY_D)) {
		camera.process_keyboard(Camera::RIGHT, delta_time_seconds);
	}
	if (window.is_key_pressed(GLFW_KEY_W)) {
		camera.process_keyboard(Camera::FORWARD, delta_time_seconds);
	}
	if (window.is_key_pressed(GLFW_KEY_E)) {
		camera.process_keyboard(Camera::DOWN, delta_time_seconds);
	}
	if (window.is_key_pressed(GLFW_KEY_Q)) {
		camera.process_keyboard(Camera::UP, delta_time_seconds);
	}

	auto extent = target.get_surface_extent();
	MVP new_mvp;
	new_mvp.model = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 1.0f));
	new_mvp.view = camera.get_view_matrix();
	new_mvp.projection = camera.get_projection_matrix(extent.width, extent.height, render_distance);
	bridge.update_mvp(new_mvp, frame);
}

void UserInterface::show_menu(float delta_time_seconds)
{
	if (is_demo_open) {
		ImGui::ShowDemoWindow(&is_demo_open);
	}

	if (dts.size() >= max_dts_size) {
		dts.erase(dts.begin());
	}
	dts.push_back(delta_time_seconds * 1000);

	if (is_menu_open) {
		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoTitleBar;
		std::string frame_time = std::string("FPS: ")
			+ std::to_string(delta_time_seconds * 1000)
			+ " ms";
		ImGui::Begin("Configuration", nullptr, window_flags);
		ImGui::PlotHistogram(frame_time.c_str(), dts.data(), dts.size(), 0, NULL, 0.0f, 60.0f);
		ImGui::SliderFloat("Render Distance", &render_distance, 10.0f, 300.0f);
		ImGui::SliderFloat("Movement Speed", &move_speed, 10.0f, 300.0f);
		ImGui::SliderFloat("Camera Speed", &look_speed, 10.0f, 300.0f);

		ImGui::End();
	}
}

}
