#include "engine/vulkan/context.h"
#include "engine/vulkan/target.h"
#include <vulkan/vulkan.h>
#include <functional>
#include <memory>

namespace kodanuki
{

/**
 * Contains all the configurable information for creating a renderer.
 *
 * The target must be specified before creating the renderer but as long
 * as the pipelines oblige with it, anything can be rendered.
 */
struct RendererBuilder
{
	// The device on which the rendering takes place.
	VulkanContext device;

	// The target which defines the rendering output.
	VulkanTarget target;

	// The color with which the frames should be cleared.
	VkClearColorValue clear_color;
};

/**
 * The vulkan renderer abstracts the render loop for rendering.
 *
 * Two frames are currently worked on at the same time. The
 * submition frame allows the user to record shapes. Once all
 * shapes are recorded, they have to be submitted. The render
 * frame contains the next frame that has finished rendering.
 *
 * Depending on the specified vulkan target multiple frames
 * can be ready for display at once. Additionally, the target
 * must be obliged.
 *
 * Instances can be copied around freely and will release all
 * ressources once unused.
 */
class VulkanRenderer
{
private:
	inline static uint64_t AQUIRE_TIMEOUT = 2 /* seconds*/ * 1000000000ull;

public:
	// Creates a new vulkan renderer from the given builder.
	VulkanRenderer(RendererBuilder builder);

public:
	// Signals that the next frame should be recorded.
	uint32_t aquire_frame();

	// Records the given shape to the command buffer.
	void draw_command(std::function<void(VkCommandBuffer)> command);

	// Submits all work to the device for rendering.
	void submit_frame(uint32_t queue_index = 0);

	// Renders the next frame that has finished rendering.
	void render_frame(uint32_t queue_index = 0);

private:
	// The abstract pointer to the implementation.
	std::shared_ptr<struct RendererState> state;
};

}
