#pragma once
#include "engine/display/vkinit.h"


namespace kodanuki::vkmath
{

/**
 * Computes "Z[i] = A[i]".
 * If inplace is true then Z == A.
 */
VulkanTensor id(VulkanDevice device, VulkanTensor tensorA, bool inplace = false);

/**
 * Computes "Z[i] = abs(A[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor abs(VulkanDevice device, VulkanTensor tensorA, bool inplace = false);

/**
 * Computes "Z[i] = sign(A[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor sign(VulkanDevice device, VulkanTensor tensorA, bool inplace = false);

/**
 * Computes "Z[i] = ceil(A[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor ceil(VulkanDevice device, VulkanTensor tensorA, bool inplace = false);

/**
 * Computes "Z[i] = floor(A[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor floor(VulkanDevice device, VulkanTensor tensorA, bool inplace = false);

/**
 * Computes "Z[i] = round(A[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor round(VulkanDevice device, VulkanTensor tensorA, bool inplace = false);

/**
 * Computes "Z[i] = clamp(A[i], B[i], C[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor clamp(VulkanDevice device, VulkanTensor tensorA, VulkanTensor tensorB, VulkanTensor tensorC, bool inplace = false);

/**
 * Computes "Z[i] = clamp(A[i], b, c)".
 * If inplace is true then Z == A.
 */
VulkanTensor clamp(VulkanDevice device, VulkanTensor tensorA, float constB, float constC, bool inplace = false);

/**
 * Computes "Z[i] = min(A[i], B[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor min(VulkanDevice device, VulkanTensor tensorA, VulkanTensor tensorB, bool inplace = false);

/**
 * Computes "Z[i] = min(A[i], b)".
 * If inplace is true then Z == A.
 */
VulkanTensor min(VulkanDevice device, VulkanTensor tensorA, float constB, bool inplace = false);

/**
 * Computes "Z[i] = max(A[i], B[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor max(VulkanDevice device, VulkanTensor tensorA, VulkanTensor tensorB, bool inplace = false);

/**
 * Computes "Z[i] = max(A[i], b)".
 * If inplace is true then Z == A.
 */
VulkanTensor max(VulkanDevice device, VulkanTensor tensorA, float constB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] + B[i]".
 * If inplace is true then Z == A.
 */
VulkanTensor add(VulkanDevice device, VulkanTensor tensorA, VulkanTensor tensorB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] + b".
 * If inplace is true then Z == A.
 */
VulkanTensor add(VulkanDevice device, VulkanTensor tensorA, float constB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] - B[i]".
 * If inplace is true then Z == A.
 */
VulkanTensor sub(VulkanDevice device, VulkanTensor tensorA, VulkanTensor tensorB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] - b".
 * If inplace is true then Z == A.
 */
VulkanTensor sub(VulkanDevice device, VulkanTensor tensorA, float constB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] * B[i]".
 * If inplace is true then Z == A.
 */
VulkanTensor mul(VulkanDevice device, VulkanTensor tensorA, VulkanTensor tensorB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] * b".
 * If inplace is true then Z == A.
 */
VulkanTensor mul(VulkanDevice device, VulkanTensor tensorA, float constB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] / B[i]".
 * If inplace is true then Z == A.
 */
VulkanTensor div(VulkanDevice device, VulkanTensor tensorA, VulkanTensor tensorB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] / b".
 * If inplace is true then Z == A.
 */
VulkanTensor div(VulkanDevice device, VulkanTensor tensorA, float constB, bool inplace = false);

/**
 * Computes "Z[i] = alpha * A[i] + beta * B[i]".
 * If inplace is true then Z == A.
 */
VulkanTensor linear(VulkanDevice device, VulkanTensor tensorA, VulkanTensor tensorB, float alpha, float beta, bool inplace = false);

/**
 * Computes "Z[i] = alpha * A[i] + beta".
 * If inplace is true then Z == A.
 */
VulkanTensor linear(VulkanDevice device, VulkanTensor tensorA, float alpha, float beta, bool inplace = false);

/**
 * Computes "Z[i] = A[i] == B[i]".
 * If inplace is true then Z == A.
 */
VulkanTensor eq(VulkanDevice device, VulkanTensor tensorA, VulkanTensor tensorB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] == b".
 * If inplace is true then Z == A.
 */
VulkanTensor eq(VulkanDevice device, VulkanTensor tensorA, float constB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] > B[i]".
 * If inplace is true then Z == A.
 */
VulkanTensor gt(VulkanDevice device, VulkanTensor tensorA, VulkanTensor tensorB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] > b".
 * If inplace is true then Z == A.
 */
VulkanTensor gt(VulkanDevice device, VulkanTensor tensorA, float constB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] >= B[i]".
 * If inplace is true then Z == A.
 */
VulkanTensor geq(VulkanDevice device, VulkanTensor tensorA, VulkanTensor tensorB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] >= b".
 * If inplace is true then Z == A.
 */
VulkanTensor geq(VulkanDevice device, VulkanTensor tensorA, float constB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] < B[i]".
 * If inplace is true then Z == A.
 */
VulkanTensor lt(VulkanDevice device, VulkanTensor tensorA, VulkanTensor tensorB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] < b".
 * If inplace is true then Z == A.
 */
VulkanTensor lt(VulkanDevice device, VulkanTensor tensorA, float constB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] <= B[i]".
 * If inplace is true then Z == A.
 */
VulkanTensor leq(VulkanDevice device, VulkanTensor tensorA, VulkanTensor tensorB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] <= b".
 * If inplace is true then Z == A.
 */
VulkanTensor leq(VulkanDevice device, VulkanTensor tensorA, float constB, bool inplace = false);

/**
 * Computes "Z[i] = sin(A[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor sin(VulkanDevice device, VulkanTensor tensorA, bool inplace = false);

/**
 * Computes "Z[i] = cos(A[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor cos(VulkanDevice device, VulkanTensor tensorA, bool inplace = false);

/**
 * Computes "Z[i] = tan(A[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor tan(VulkanDevice device, VulkanTensor tensorA, bool inplace = false);

/**
 * Computes "Z[i] = exp(A[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor exp(VulkanDevice device, VulkanTensor tensorA, bool inplace = false);

/**
 * Computes "Z[i] = log(A[i])" with base e.
 * If inplace is true then Z == A.
 */
VulkanTensor log(VulkanDevice device, VulkanTensor tensorA, bool inplace = false);

/**
 * Computes "Z[i] = pow(A[i], B[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor pow(VulkanDevice device, VulkanTensor tensorA, VulkanTensor tensorB, bool inplace = false);

/**
 * Computes "Z[i] = pow(A[i], b)".
 * If inplace is true then Z == A.
 */
VulkanTensor pow(VulkanDevice device, VulkanTensor tensorA, float constB, bool inplace = false);

/**
 * Computes "A[i] = range(start, stop)[i]".
 * Always inplace, output is the input tensor.
 */
VulkanTensor range(VulkanDevice device, VulkanTensor tensorA, float start, float step);

/**
 * Computes "A[i] = b".
 * Always inplace, output is the input tensor.
 */
VulkanTensor fill(VulkanDevice device, VulkanTensor tensorA, float constB);

}
