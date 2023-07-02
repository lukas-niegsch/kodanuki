#pragma once
#include "engine/central/entity.h"
#include <utility>
class GLFWwindow;

namespace kodanuki
{

/**
 * The window is a wrapper around a GLFW window.
 * 
 * It handles the interaction with the OS such as the actual window,
 * user input, and rendering. Some features are not yet implemented
 * for simplicity such as multiple monitor support.
 * 
 * The class can be used with the builder pattern:
 *
 * Window window = Window::create({
 *     .title = "MyTitle",
 *     .shape = {600, 600},
 *     // other fields
 * });
 *
 * This makes sure that the window is in the correct state. Afterward,
 * some properties can be changed via getter and setter methods.
 */
class Window
{
public:
    /**
     * The context type for the client API.
     */
    enum ClientType {
        Vulkan = 1 << 1,
        OpenGL = 1 << 2
    };
    
    /**
     * The flags for the window style.
     */
    enum StyleFlags {
        Decorated  = 1 << 1,
        Resizable  = 1 << 2,
        Fullscreen = 1 << 3
    };

public:
    /**
     * The parameters that are used to build a new window.
     * 
     * Some parameters might be ignored depending on the input. For
     * example the width, height and title while in fullscreen mode.
     * The user should make sure that these values make sense, for
     * example that the width is not to high.
     *
     * Currently the window will always be visible and created in the
     * center of the screen. The used monitor will be the default one
     * specified by GLFW.
     */
    struct WindowBuilder
    {
        /**
         * The owning entity for this window. If specified the window
         * will be destroyed once the owner is deleted. This parameter
         * is optional.
         */
        Entity owner;

        /**
         * The client types enables the context for different client
         * API's such as OpenGL. Enable the flags if you intend to use
         * one of them. Multiple types can be used together.
         */
        uint32_t dtype;

        /**
         * The title for the window. It will only be visible if the
         * window is decorated and not in fullscreen. This parameter
         * is optional.
         */
        const char* title;

        /**
         * The icon for the window. It is also only visible if the
         * window is decorated and not in fullscreen. Invalid paths
         * will be ignored. This parameter is optional.
         */
        const char* icon;

        /**
         * The style flags for the window. Some flags might overwrite
         * others, e.g. Moveable and Fullscreen. This parameter is
         * optional.
         */
        uint32_t flags;

        /**
         * The default width and height of the window. Please make sure
         * that these values don't become too large. They are given in
         * screen pixels.
         */
        std::pair<uint32_t, uint32_t> shape;

        /**
         * Maximum number of frames this window should be updated in
         * one second. The window will sleep when it ticks more often
         * than this. The default 0 will be interpreted as 60 fps.
         */
        uint32_t frames;
    };

    /**
     * Creates and initializes a new window from the builder.
     *
     * This function makes sure that all required setters are called
     * and the window is in a correct state.
     *
     * @param builder The options for the window creation.
     */
    static Window create(WindowBuilder builder);

public:
    Window();
    ~Window();

    operator GLFWwindow*();
    bool tick();

public:
    bool should_close();

    void set_title(const char* title);
    void set_icon(const char* icon);
    void set_decorated(bool decorated);
    void set_fullscreen(bool fullscreen);
    void set_frames(uint32_t frames);
    void set_resizable(bool resizable);

private:
    void initialize(uint32_t dtype, std::pair<uint32_t, uint32_t> shape);


private:
    // The entity that stores the the implementation.
    Entity impl;
};

}
