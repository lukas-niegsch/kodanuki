/**
 * Just an example to test out the fragment shaders.
 *
 * This shader simply passes the colors the fragment with the given color.
 */
#version 450

layout(location = 0) in vec3 inFragColor;
layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(inFragColor, 1.0);
}
