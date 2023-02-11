#include "source/splash/user_interface.h"
#include "source/splash/user_interface_backend.h"
#include "engine/vulkan/debug.h"
#include "engine/vulkan/utility.h"
#include <vector>

namespace kodanuki
{

std::vector<VkCommandBuffer> create_command_buffers(VkDevice device, VkCommandPool pool, uint32_t count)
{
	VkCommandBufferAllocateInfo buffer_info = {};
	buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	buffer_info.commandPool = pool;
	buffer_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	buffer_info.commandBufferCount = count;

	std::vector<VkCommandBuffer> result(count);
	CHECK_VULKAN(vkAllocateCommandBuffers(device, &buffer_info, result.data()));
	return result;
}

VkCommandPool create_command_pool(VkDevice device, uint32_t queue_index)
{
	VkCommandPoolCreateInfo pool_info;
	pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_info.pNext = nullptr;
	pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	pool_info.queueFamilyIndex = queue_index;

	VkCommandPool command_pool;
	CHECK_VULKAN(vkCreateCommandPool(device, &pool_info, nullptr, &command_pool));
	return command_pool;
}

struct UserInterfaceState
{
	VulkanDevice device;
	VulkanWindow window;
	VkDescriptorPool imgui_pool;
	~UserInterfaceState();
};

UserInterfaceState::~UserInterfaceState()
{
	CHECK_VULKAN(vkDeviceWaitIdle(device));
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	vkDestroyDescriptorPool(device, imgui_pool, nullptr);
	ImGui::DestroyContext();
}

UserInterface::UserInterface(UserInterfaceBuilder builder)
{
	state = std::make_shared<UserInterfaceState>(builder.device, builder.window);
	state->imgui_pool = create_descriptor_pool(builder.device);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF(builder.fontfile.c_str(), builder.fontsize);

	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForVulkan(state->window, true);

	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = builder.device.instance();
	init_info.PhysicalDevice = builder.device.physical_device();
	init_info.Device = builder.device;
	init_info.QueueFamily = builder.device.queue_family_index();
	init_info.Queue = builder.device.queues()[1];
	init_info.DescriptorPool = state->imgui_pool;
	init_info.MinImageCount = 2;
	init_info.ImageCount = builder.target.get_frame_count();
	ImGui_ImplVulkan_Init(&init_info, builder.target.renderpass());

	VkCommandPool command_pool = create_command_pool(builder.device, builder.device.queue_family_index());
	VkCommandBuffer command_buffer = create_command_buffers(builder.device, command_pool, 1)[0];

	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	CHECK_VULKAN(vkBeginCommandBuffer(command_buffer, &begin_info));
    ImGui_ImplVulkan_CreateFontsTexture(command_buffer);
	CHECK_VULKAN(vkEndCommandBuffer(command_buffer));

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &command_buffer;
	CHECK_VULKAN(vkQueueSubmit(init_info.Queue, 1, &submit_info, VK_NULL_HANDLE));
	
	CHECK_VULKAN(vkDeviceWaitIdle(builder.device));
	ImGui_ImplVulkan_DestroyFontUploadObjects();
	vkDestroyCommandPool(builder.device, command_pool, nullptr);
}

bool UserInterface::tick()
{
	static bool first_frame = true;

	if (!first_frame) {
		ImGui::Render();
	} else {
		first_frame = false;
	}
	
	bool running = state->window.tick();

	if (!running) {
		return running;
	}

	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	return running;
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
		std::string frame_time = std::string("Frame: ") + std::to_string(delta_time * 1000) + " ms";

		ImGui::Begin("Configuration", nullptr, window_flags);
		ImGui::PlotHistogram(frame_time.c_str(), config.dts.data(), config.dts.size(), 0, NULL, 0.0f, 60.0f);
		ImGui::SliderFloat("Render Distance", &config.render_distance, 10.0f, 300.0f);
		ImGui::SliderFloat("Movement Speed", &config.move_speed, 10.0f, 300.0f);
		ImGui::SliderFloat("Camera Speed", &config.look_speed, 10.0f, 300.0f);
		ImGui::End();
	}
}

}
