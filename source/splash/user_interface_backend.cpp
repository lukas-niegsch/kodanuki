#include "source/splash/user_interface_backend.h"

/**
 * I currently can't add individual source files to the build system.
 * This is because I can only add folders and the build system searches
 * for all cpp files in the directory. However, this is not possible
 * here because ImGui implements many backends and I only want to add
 * the vulkan and glfw one. To avoid this we add all source files into
 * one and compile it here.
 */
#include "extern/imgui/imgui.cpp"
#include "extern/imgui/imgui_demo.cpp"
#include "extern/imgui/imgui_draw.cpp"
#include "extern/imgui/imgui_tables.cpp"
#include "extern/imgui/imgui_widgets.cpp"
#include "extern/imgui/backends/imgui_impl_vulkan.cpp"
#include "extern/imgui/backends/imgui_impl_glfw.cpp"
