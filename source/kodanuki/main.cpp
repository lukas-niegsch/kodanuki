#include "engine/central/entity.h"
#include "engine/central/archetype.h"
#include "engine/display/layout.h"
#include <GLFW/glfw3.h>	
#include <vector>
#include <cstdint>
#include <cstring>
#include <iostream>
using namespace kodanuki;

void render_layouts(uint32_t width, uint32_t height)
{
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(width, height, "Render Layouts", nullptr, nullptr);
	if (window == nullptr)
	{
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(window);

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		glOrtho(0, width, 0, height, -1, 1);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glColor3ub(255, 0, 0);

		for (auto[layout] : ECS::iterate<Archetype<Iterate<RenderLayout>>>())
		{
			glBegin(GL_LINE_LOOP);

			int l = layout.xpos;
			int r = layout.xpos + layout.width;
			int t = height - layout.ypos;
			int b = height - layout.ypos - layout.height;

			glVertex2f(l, t);
			glVertex2f(r, t);
			glVertex2f(r, b);
			glVertex2f(l, b);
			glEnd();
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
}

int main()
{
	Entity root = ECS::create(); (void) root;

	Entity entityA = ECS::create();
	Entity entityB = ECS::create();
	Entity entityC = ECS::create();

	ECS::update<RenderLayout>(entityA, {0, 10, 10, 100, 50});
	ECS::update<RenderLayout>(entityB, {0, 50, 70, 80, 100});
	ECS::update<RenderLayout>(entityC, {0, 20, 30, 60, 120});

	// ECS::update<RenderLayout>(root, {0, 10, 10, 580, 580});
	render_layouts(600, 600);

	return 0;
}
