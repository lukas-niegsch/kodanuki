#pragma once
#include <GL/glew.h>
#include <GL/gl.h>
#include <string>

struct OpenglShader
{
	std::string vertfile;
	std::string fragfile;
};

/**
 * Loads the opengl shader into the gpu for the current context.
 * 
 * @param shader The files containing the vertex and frament shader.
 */
GLuint load_shader(OpenglShader shader);

