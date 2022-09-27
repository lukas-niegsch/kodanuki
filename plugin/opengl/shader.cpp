#include "plugin/opengl/shader.h"
#include "engine/utility/file.h"
using namespace Kodanuki;
#include <iostream>

void check_compile_status(GLuint entityID)
{
	GLint success = GL_FALSE;
	int log_length;
	glGetShaderiv(entityID, GL_COMPILE_STATUS, &success);
	glGetShaderiv(entityID, GL_INFO_LOG_LENGTH, &log_length);
	if (!success && log_length > 0) {
		std::vector<char> message(log_length + 1);
		glGetShaderInfoLog(entityID, log_length, nullptr, message.data());
		printf("%s\n", message.data());
	}
}

void check_link_status(GLuint entityID)
{
	GLint success = GL_FALSE;
	int log_length;
	glGetProgramiv(entityID, GL_LINK_STATUS, &success);
	glGetProgramiv(entityID, GL_INFO_LOG_LENGTH, &log_length);
	if (!success && log_length > 0) {
		std::vector<char> message(log_length + 1);
		glGetProgramInfoLog(entityID, log_length, nullptr, message.data());
		printf("%s\n", message.data());
	}
}

GLuint load_shader(OpenglShader shader)
{
	std::vector<char> vert_code = read_file_into_buffer(shader.vertfile);
	std::vector<char> frag_code = read_file_into_buffer(shader.fragfile);
	const char* vert_ptr = vert_code.data();
	const char* frag_ptr = frag_code.data();

	GLuint vertID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragID = glCreateShader(GL_FRAGMENT_SHADER);
	
	glShaderSource(vertID, 1, &vert_ptr, nullptr);
	glCompileShader(vertID);
	check_compile_status(vertID);

	glShaderSource(fragID, 1, &frag_ptr, nullptr);
	glCompileShader(fragID);
	check_compile_status(fragID);

	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertID);
	glAttachShader(programID, fragID);
	glLinkProgram(programID);
	check_link_status(programID);

	glDetachShader(programID, vertID);
	glDetachShader(programID, fragID);
	glDeleteShader(vertID);
	glDeleteShader(fragID);

	return programID;
}

