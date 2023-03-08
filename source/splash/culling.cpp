#include "source/splash/culling.h"

namespace kodanuki
{

VulkanTensor cull_invisible_spheres(VulkanTensor spheres, VulkanTensor mvp, float p, float q)
{
	(void) spheres; (void) mvp; (void) p; (void) q;
	return spheres;
}

}