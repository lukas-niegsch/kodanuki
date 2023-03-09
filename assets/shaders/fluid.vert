/**
 * The vertex shader used for fluid simulation.
 *
 * The shader uses instances to draw each particle. Each particle
 * can have a different color. The shader also handles the MVP
 * matrix that transforms the model coordinates to the projection
 * coordinates.
 */
#version 450

layout(binding = 0) uniform MVP {
    mat4 model;
    mat4 view;
    mat4 proj;
} mvp;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inUV;
layout(location = 4) in vec3 inInstancePosition;

layout(location = 0) out vec3 outFragColor;

const vec3 LIGHT_DIRECTION = normalize(vec3(0.0, 1.0, 0.0));
const float LIGHT_AMBIANT = 0.2;

void main()
{
    gl_Position = mvp.proj * mvp.view * mvp.model * vec4(inInstancePosition + inPosition, 1.0);
    mat3 normalMatrix = transpose(inverse(mat3(mvp.model)));
    vec3 normalWorldSpace = normalize(normalMatrix * inNormal);
    vec3 lightWorldSpace = LIGHT_DIRECTION;
    float lightIntensity = clamp(dot(normalWorldSpace, lightWorldSpace), 0.1, 0.7);
    outFragColor = lightIntensity * inColor;
}
