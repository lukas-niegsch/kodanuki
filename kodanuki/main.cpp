#include "engine/central/archetype.h"
#include "engine/central/entity.h"
#include "engine/concept/polygon.h"
#include "engine/renderer/camera.h"
#include "engine/defines/keys.h"
#include "engine/defines/location.h"
#include "plugin/opengl/shader.h"
#include "plugin/vulkan/input.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
using namespace Kodanuki;

constexpr GLfloat cube_strip[] = {
	 1,  1,  1,
	-1,  1,  1,
	 1, -1,  1,
	-1, -1,  1,
	-1, -1, -1,
	-1,  1,  1,
	-1,  1, -1,
	 1,  1,  1,
	 1,  1, -1,
	 1, -1,  1,
	 1, -1, -1,
	-1, -1, -1,
	 1,  1, -1,
	-1,  1, -1
};

int main()
{
	int width = 1024;
	int height = 768;

	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_strip), cube_strip, GL_STATIC_DRAW);
	
	GLuint programID = load_shader({"shader/example.vert", "shader/example.frag"});
	glClearColor(0.0f, 1.0f, 1.0f, 0.0f);

	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float) width / (float) height, 0.1f, 100.0f);
	glm::mat4 Model = glm::mat4(1.0f);
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	
	Entity player = ECS::create();
	ECS::update<Camera>(player);
	ECS::update<Location>(player, {{5, 0, 0}, {-1, 0, 0}});
	ECS::update<KeyReceiver>(player);

	float horizontalAngle = 0.00f;
	float verticalAngle = 0.0f;
	float speed = 0.5f;
	float mouseSpeed = 0.003f;

	bool running = true;
	while(running && !glfwWindowShouldClose(window)) {
		system_update_key_receiver<KEY_ESCAPE>(window);
		system_update_key_receiver<KEY_W>(window);
		system_update_key_receiver<KEY_A>(window);
		system_update_key_receiver<KEY_S>(window);
		system_update_key_receiver<KEY_D>(window);
		system_update_key_receiver<KEY_LEFT_SHIFT>(window);
		system_update_key_receiver<KEY_SPACE>(window);

		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		glfwSetCursorPos(window, width / 2, height / 2);

		horizontalAngle -= mouseSpeed * float(width / 2 - xpos);
		verticalAngle   -= mouseSpeed * float(height / 2 - ypos);

		Location& transform = ECS::get<Location>(player);

		transform.direction = {
    		cos(verticalAngle) * sin(horizontalAngle),
    		sin(verticalAngle),
    		cos(verticalAngle) * cos(horizontalAngle)
		};

		glm::vec3 forward = glm::normalize(transform.direction);
		glm::vec3 left = glm::cross(forward, {0.0f, 1.0f, 0.0f});
		left = glm::normalize(left);

		if (ECS::has<KEY_ESCAPE>(player)) {
			running = false;
			continue;
		}

		if (ECS::has<KEY_W>(player)) {
			transform.position += forward * speed;
		}
		
		if (ECS::has<KEY_S>(player)) {
			transform.position -= forward * speed;
		}

		if (ECS::has<KEY_D>(player)) {
			transform.position -= left * speed;
		}

		if (ECS::has<KEY_A>(player)) {
			transform.position += left * speed;
		}

		if (ECS::has<KEY_LEFT_SHIFT>(player)) {
			transform.position.y += speed;
		}

		if (ECS::has<KEY_SPACE>(player)) {
			transform.position.y -= speed;
		}

		ECS::remove<CameraView>(player);
		execute_camera_system();
		glm::mat4 view = ECS::get<CameraView>(player).view_matrix;
		glm::mat4 mvp = Projection * view * Model;
		
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(programID);
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 14); // Starting from vertex 0; 3 vertices total -> 1 triangle
		glDisableVertexAttribArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

