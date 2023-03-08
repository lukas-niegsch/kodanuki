#pragma once
#include "engine/vulkan/tensor.h"

namespace kodanuki
{

/**
 * Removes all the spheres that should not be visible.
 * 
 * Steps:
 * 1. Remove spheres outside the View Frustum (Frustum Culling).
 * 2. Project all spheres onto the nearest plane and sort them by size.
 * 3. Remove all spheres whose projection ellipsis is contained inside another.
 *
 * We do not care for absolute accuracy inside the third step. We are happy if
 * we can remove many spheres. We first compute the size of the ellipses and
 * then select the smallest p% and check if they are contained inside the
 * biggest q%. For p = q = 1 this would give us the optimal solution, but for
 * other p, q this approach allows us to sacrafice accuracy for performance.
 *
 * @param spheres The tensor containing the sphere positions.
 * @param mvp The tensor containing the model view projection matrices.
 * @param p The percentage of the smallest spheres that will be checked.
 * @param q The percentage of the biggest spheres that will be checked against.
 * @return The tensor containing the positions of all unculled spheres.
 */
VulkanTensor cull_invisible_spheres(VulkanTensor spheres, VulkanTensor mvp, float p = 0.3, float q = 0.3);

}
