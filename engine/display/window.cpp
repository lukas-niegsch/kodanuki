#include "engine/display/window.h"
#include "extern/stb/stb_image.h"
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <iostream>

namespace kodanuki
{

static uint32_t window_counter = 0;

struct WindowState
{

};

Window Window::create(WindowBuilder builder)
{
    Window window;
    window.impl = ECS::create(builder.owner);
    window.initialize(builder.dtype, builder.shape);
    window.set_title(builder.title);
    window.set_icon(builder.icon);
    window.set_decorated(builder.flags & Window::Decorated);
    window.set_fullscreen(builder.flags & Window::Fullscreen);
    window.set_frames(builder.frames);
    window.set_resizable(builder.flags & Window::Resizable);
    return window;
}

Window::Window()
{
    if (window_counter == 0) {
        glfwInit();
    }
    window_counter++;
}

Window::~Window()
{
    if (window_counter == 1) {
        glfwTerminate();
    }
    window_counter--;
}

Window::operator GLFWwindow*()
{
    return ECS::get<GLFWwindow*>(impl);
}

bool Window::tick()
{
    return !should_close();
}

bool Window::should_close()
{
	glfwPollEvents();
	return glfwWindowShouldClose(*this);
}

void Window::set_title(const char* title)
{
    glfwSetWindowTitle(*this, title);
}

void Window::set_icon(const char* icon)
{
    GLFWimage images; 
    images.pixels = stbi_load(icon, &images.width, &images.height, 0, 4);
    glfwSetWindowIcon(*this, 1, &images); 
    stbi_image_free(images.pixels);
}

void Window::set_decorated(bool decorated)
{
    glfwSetWindowAttrib(*this, GLFW_DECORATED, decorated);
}

void Window::set_fullscreen(bool fullscreen)
{
    (void) fullscreen;
}

void Window::set_frames(uint32_t frames)
{
    (void) frames;
}

void Window::initialize(uint32_t dtype, std::pair<uint32_t, uint32_t> shape)
{
    int width  = shape.first;
    int height = shape.second;

    (void) dtype;
    (void) shape;


    auto window = glfwCreateWindow(width, height, "", NULL, NULL);
    ECS::update(impl, window);
}

void Window::set_resizable(bool resizable)
{
    glfwSetWindowAttrib(*this, GLFW_RESIZABLE, resizable);
}

}
