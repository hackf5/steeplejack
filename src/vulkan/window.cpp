#include "vulkan/window.h"

#include <stdexcept>

#include "spdlog/spdlog.h"

using namespace steeplejack;

Window::Window(int width, int height, const std::string& title)
    : width_(width), height_(height) {
    spdlog::info("Creating Window Components");

    if (glfwInit() != GLFW_TRUE) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window_ = glfwCreateWindow(width_, height_, title.c_str(), nullptr, nullptr);
    if (!window_) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
}

Window::~Window() {
    spdlog::info("Destroying Window Components");
    if (window_) glfwDestroyWindow(window_);
    glfwTerminate();
}

void Window::framebuffer_resize_callback(GLFWwindow* window, int width, int height) {
    auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    if (app && app->framebuffer_resize_callback_) {
        app->framebuffer_resize_callback_(width, height);
    }
}

VkSurfaceKHR Window::create_window_surface(VkInstance instance) const {
    spdlog::info("Creating Window Surface");
    VkSurfaceKHR surface{};
    if (glfwCreateWindowSurface(instance, window_, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface");
    }
    return surface;
}

void Window::register_framebuffer_resize_callback(framebuffer_resize_callback_t callback) {
    framebuffer_resize_callback_ = callback;
}

void Window::wait_resize() {
    glfwGetFramebufferSize(window_, &width_, &height_);
    while (width_ == 0 || height_ == 0) {
        glfwGetFramebufferSize(window_, &width_, &height_);
        glfwWaitEvents();
    }
}

