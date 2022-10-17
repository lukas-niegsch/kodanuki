#include "plugin/vulkan/pipeline.h"

namespace Kodanuki
{

void remove_pipeline(Entity* pipeline)
{
	ECS::remove<Entity>(*pipeline);
	delete pipeline;
}

VulkanPipeline::VulkanPipeline(PipelineBuilder builder)
{
	pimpl = std::shared_ptr<Entity>(new Entity, &remove_pipeline);
	Entity pipeline = *pimpl = ECS::create();

    ECS::update<VulkanDevice>(pipeline, builder.device);
}

}
