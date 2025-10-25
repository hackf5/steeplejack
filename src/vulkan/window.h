#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <functional>
#include <string>

#include "util/no_copy_or_move.h"

namespace steeplejack {

class Window : NoCopyOrMove {
public:
    using framebuffer_resize_callback_t = std::function<void(int, int)>;

private:
    int width_;
    int height_;
    framebuffer_resize_callback_t framebuffer_resize_callback_;
    GLFWwindow* window_{};

    static void framebuffer_resize_callback(GLFWwindow* window, int width, int height);

public:
    Window(int width, int height, const std::string& title);
    ~Window();

    VkSurfaceKHR create_window_surface(VkInstance instance) const;

    operator GLFWwindow*() const { return window_; }

    void register_framebuffer_resize_callback(framebuffer_resize_callback_t callback);
    bool should_close() const { return glfwWindowShouldClose(window_); }
    void poll_events() const { glfwPollEvents(); }
    void wait_resize();
};

} // namespace steeplejack

