#include "engine/central/archetype.h"
#include "engine/central/entity.h"
#include "engine/display/layout.h"
#include <GLFW/glfw3.h>
using namespace kodanuki;

class DebugSystem
{
public:
	DebugSystem();

public:
	void game_loop();

private:
	void create_debug_layout();
	void render_debug_layout();

private:
	uint32_t width;
	uint32_t height;
	Entity root;

	LayoutSystem layouts;
};

int main()
{
	DebugSystem system;
	system.game_loop();
	return 0;
}

DebugSystem::DebugSystem() : width(600), height(600), root(ECS::create())
{
	layouts = LayoutSystem::create({
		.owner = root
	});

	create_debug_layout();
}

void DebugSystem::game_loop()
{
	layouts.tick();
	render_debug_layout();
}

void DebugSystem::create_debug_layout()
{
	Entity entityA = ECS::create(root);
	Entity entityB = ECS::create(root);
	Entity entityC = ECS::create(root);

	// The root must have a predefined render layout!
	ECS::update<RenderLayout>(root, {
		.xpos = 10,
		.ypos = 10,
		.width = width - 20,
		.height = height - 20
	});
	
	ECS::update<IsLayoutFlag>(root);
	ECS::update<RenderLayout>(entityA, {10, 10, 100, 50});
	ECS::update<RenderLayout>(entityB, {50, 70, 80, 100});
	ECS::update<RenderLayout>(entityC, {20, 30, 60, 120});
}

void DebugSystem::render_debug_layout()
{
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(width, height, "Render Layouts", nullptr, nullptr);
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
