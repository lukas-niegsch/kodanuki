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

void main() {
    gl_Position = vec4(inPosition, 1.0);
    fragColor = inColor;
}