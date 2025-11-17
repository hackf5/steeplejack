#pragma once

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <functional>
#include <string>

namespace steeplejack
{
class Window
{
  public:
    typedef std::function<void(int, int)> framebuffer_resize_callback_t;

  private:
    int m_width;
    int m_height;

    framebuffer_resize_callback_t m_framebuffer_resize_callback;

    GLFWwindow* m_window;

    static void framebuffer_resize_callback(GLFWwindow* window, int width, int height);

  public:
    Window(int width, int height, const std::string& title);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    VkSurfaceKHR create_window_surface(VkInstance instance) const;

    operator GLFWwindow*() const
    {
        return m_window;
    }

    void register_framebuffer_resize_callback(framebuffer_resize_callback_t callback);

    bool should_close() const
    {
        return glfwWindowShouldClose(m_window);
    }

    void poll_events() const
    {
        glfwPollEvents();
    }

    void wait_resize();
};
} // namespace steeplejack
