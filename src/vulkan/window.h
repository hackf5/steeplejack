#pragma once

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <string>

namespace steeplejack
{
class Window
{
  private:
    int m_width;
    int m_height;

    GLFWwindow* m_window;

  public:
    Window(int width, int height, const std::string& title);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    [[nodiscard]] GLFWwindow* glfw() const;

    [[nodiscard]] VkSurfaceKHR create_window_surface(VkInstance instance) const;

    void poll_events() const; // NOLINT(readability-convert-member-functions-to-static)

    void wait_resize();

    [[nodiscard]] bool should_close() const;
};
} // namespace steeplejack
