#pragma once
#include "engine/display/vkinit.h"


namespace kodanuki::vkmath
{

/**
 * Computes "Z[i] = A[i]".
 * If inplace is true then Z == A.
 */
VulkanTensor id(VulkanTensor tensorA, bool inplace = false);

/**
 * Computes "Z[i] = abs(A[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor abs(VulkanTensor tensorA, bool inplace = false);

/**
 * Computes "Z[i] = sign(A[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor sign(VulkanTensor tensorA, bool inplace = false);

/**
 * Computes "Z[i] = ceil(A[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor ceil(VulkanTensor tensorA, bool inplace = false);

/**
 * Computes "Z[i] = floor(A[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor floor(VulkanTensor tensorA, bool inplace = false);

/**
 * Computes "Z[i] = round(A[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor round(VulkanTensor tensorA, bool inplace = false);

/**
 * Computes "Z[i] = clamp(A[i], B[i], C[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor clamp(VulkanTensor tensorA, VulkanTensor tensorB, VulkanTensor tensorC, bool inplace = false);

/**
 * Computes "Z[i] = clamp(A[i], b, c)".
 * If inplace is true then Z == A.
 */
VulkanTensor clamp(VulkanTensor tensorA, float constB, float constC, bool inplace = false);

/**
 * Computes "Z[i] = min(A[i], B[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor min(VulkanTensor tensorA, VulkanTensor tensorB, bool inplace = false);

/**
 * Computes "Z[i] = min(A[i], b)".
 * If inplace is true then Z == A.
 */
VulkanTensor min(VulkanTensor tensorA, float constB, bool inplace = false);

/**
 * Computes "Z[i] = max(A[i], B[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor max(VulkanTensor tensorA, VulkanTensor tensorB, bool inplace = false);

/**
 * Computes "Z[i] = max(A[i], b)".
 * If inplace is true then Z == A.
 */
VulkanTensor max(VulkanTensor tensorA, float constB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] + B[i]".
 * If inplace is true then Z == A.
 */
VulkanTensor add(VulkanTensor tensorA, VulkanTensor tensorB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] + b".
 * If inplace is true then Z == A.
 */
VulkanTensor add(VulkanTensor tensorA, float constB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] - B[i]".
 * If inplace is true then Z == A.
 */
VulkanTensor sub(VulkanTensor tensorA, VulkanTensor tensorB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] - b".
 * If inplace is true then Z == A.
 */
VulkanTensor sub(VulkanTensor tensorA, float constB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] * B[i]".
 * If inplace is true then Z == A.
 */
VulkanTensor mul(VulkanTensor tensorA, VulkanTensor tensorB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] * b".
 * If inplace is true then Z == A.
 */
VulkanTensor mul(VulkanTensor tensorA, float constB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] / B[i]".
 * If inplace is true then Z == A.
 */
VulkanTensor div(VulkanTensor tensorA, VulkanTensor tensorB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] / b".
 * If inplace is true then Z == A.
 */
VulkanTensor div(VulkanTensor tensorA, float constB, bool inplace = false);

/**
 * Computes "Z[i] = alpha * A[i] + beta * B[i]".
 * If inplace is true then Z == A.
 */
VulkanTensor linear(VulkanTensor tensorA, VulkanTensor tensorB, float alpha, float beta, bool inplace = false);

/**
 * Computes "Z[i] = alpha * A[i] + beta".
 * If inplace is true then Z == A.
 */
VulkanTensor linear(VulkanTensor tensorA, float alpha, float beta, bool inplace = false);

/**
 * Computes "Z[i] = A[i] == B[i]".
 * If inplace is true then Z == A.
 */
VulkanTensor eq(VulkanTensor tensorA, VulkanTensor tensorB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] == b".
 * If inplace is true then Z == A.
 */
VulkanTensor eq(VulkanTensor tensorA, float constB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] > B[i]".
 * If inplace is true then Z == A.
 */
VulkanTensor gt(VulkanTensor tensorA, VulkanTensor tensorB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] > b".
 * If inplace is true then Z == A.
 */
VulkanTensor gt(VulkanTensor tensorA, float constB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] >= B[i]".
 * If inplace is true then Z == A.
 */
VulkanTensor geq(VulkanTensor tensorA, VulkanTensor tensorB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] >= b".
 * If inplace is true then Z == A.
 */
VulkanTensor geq(VulkanTensor tensorA, float constB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] < B[i]".
 * If inplace is true then Z == A.
 */
VulkanTensor lt(VulkanTensor tensorA, VulkanTensor tensorB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] < b".
 * If inplace is true then Z == A.
 */
VulkanTensor lt(VulkanTensor tensorA, float constB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] <= B[i]".
 * If inplace is true then Z == A.
 */
VulkanTensor leq(VulkanTensor tensorA, VulkanTensor tensorB, bool inplace = false);

/**
 * Computes "Z[i] = A[i] <= b".
 * If inplace is true then Z == A.
 */
VulkanTensor leq(VulkanTensor tensorA, float constB, bool inplace = false);

/**
 * Computes "Z[i] = sin(A[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor sin(VulkanTensor tensorA, bool inplace = false);

/**
 * Computes "Z[i] = cos(A[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor cos(VulkanTensor tensorA, bool inplace = false);

/**
 * Computes "Z[i] = tan(A[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor tan(VulkanTensor tensorA, bool inplace = false);

/**
 * Computes "Z[i] = exp(A[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor exp(VulkanTensor tensorA, bool inplace = false);

/**
 * Computes "Z[i] = log(A[i])" with base e.
 * If inplace is true then Z == A.
 */
VulkanTensor log(VulkanTensor tensorA, bool inplace = false);

/**
 * Computes "Z[i] = pow(A[i], B[i])".
 * If inplace is true then Z == A.
 */
VulkanTensor pow(VulkanTensor tensorA, VulkanTensor tensorB, bool inplace = false);

/**
 * Computes "Z[i] = pow(A[i], b)".
 * If inplace is true then Z == A.
 */
VulkanTensor pow(VulkanTensor tensorA, float constB, bool inplace = false);

/**
 * Computes "A[i] = range(start, stop)[i]".
 * Always inplace, output is the input tensor.
 */
VulkanTensor range(VulkanTensor tensorA, float start, float step);

/**
 * Computes "A[i] = b".
 * Always inplace, output is the input tensor.
 */
VulkanTensor fill(VulkanTensor tensorA, float constB);

}
