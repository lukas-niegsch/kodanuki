#include "source/splash/user_interface.h"
#include "source/splash/user_interface_backend.h"
#include "engine/vulkan/debug.h"
#include <vector>

namespace kodanuki
{

struct UserInterfaceState
{
	VulkanDevice device;
	VulkanWindow window;
	~UserInterfaceState();
};

UserInterfaceState::~UserInterfaceState()
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

UserInterface::UserInterface(UserInterfaceBuilder builder)
{
	state = std::make_shared<UserInterfaceState>(builder.device, builder.window);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF(builder.fontfile.c_str(), builder.fontsize);

	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForVulkan(builder.window, true);

	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = builder.device.get_instance();
	init_info.PhysicalDevice = builder.device.get_physical_device();
	init_info.Device = builder.device;
	init_info.QueueFamily = builder.device.get_queue_family();
	init_info.Queue = builder.device.get_queues()[1];
	init_info.DescriptorPool = builder.device.get_descriptor_pool();
	init_info.MinImageCount = 2;
	init_info.ImageCount = builder.target.get_frame_count();
	ImGui_ImplVulkan_Init(&init_info, builder.target.renderpass());

	builder.device.execute([&](VkCommandBuffer buffer) {
		ImGui_ImplVulkan_CreateFontsTexture(buffer);
	});
	ImGui_ImplVulkan_DestroyFontUploadObjects();
}

bool UserInterface::tick()
{
	if (!state->window.tick()) {
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

void show_config(Config& config, float delta_time)
{
	if (config.is_demo_open) {
		ImGui::ShowDemoWindow(&config.is_demo_open);
	}

	if (config.dts.size() >= config.max_dts_size) {
		config.dts.erase(config.dts.begin());
	}
	config.dts.push_back(delta_time * 1000);

	if (config.is_menu_open) {
		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoTitleBar;
		std::string frame_time = std::string("FPS: ")
			+ std::to_string(delta_time * 1000)
			+ " ms";
		ImGui::Begin("Configuration", nullptr, window_flags);
		ImGui::PlotHistogram(frame_time.c_str(), config.dts.data(), config.dts.size(), 0, NULL, 0.0f, 60.0f);
		ImGui::SliderFloat("Render Distance", &config.render_distance, 10.0f, 300.0f);
		ImGui::SliderFloat("Movement Speed", &config.move_speed, 10.0f, 300.0f);
		ImGui::SliderFloat("Camera Speed", &config.look_speed, 10.0f, 300.0f);

		ImGui::End();
	}
}

}
