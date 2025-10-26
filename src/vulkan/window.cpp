#include "window.h"

#include <spdlog/spdlog.h>
#include <stdexcept>
#include <utility>

using namespace steeplejack;

Window::Window(int width, int height, const std::string& title) : m_width(width), m_height(height)
{
    spdlog::info("Creating Window Components");

    try
    {
        if (glfwInit() != GLFW_TRUE)
        {
            throw std::runtime_error("Failed to initialize GLFW");
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        m_window = glfwCreateWindow(m_width, m_height, title.c_str(), nullptr, nullptr);
    }
    catch (const std::exception& e)
    {
        this->~Window();
        throw;
    }
}

Window::~Window()
{
    spdlog::info("Destroying Window Components");
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Window::framebuffer_resize_callback(GLFWwindow* window, int width, int height)
{
    auto* app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));

    if (app->m_framebuffer_resize_callback)
    {
        app->m_framebuffer_resize_callback(width, height);
    }
}

VkSurfaceKHR Window::create_window_surface(VkInstance instance) const
{
    spdlog::info("Creating Window Surface");

    VkSurfaceKHR surface = nullptr;
    if (glfwCreateWindowSurface(instance, m_window, nullptr, &surface) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create window surface");
    }

    return surface;
}

void Window::register_framebuffer_resize_callback(framebuffer_resize_callback_t callback)
{
    m_framebuffer_resize_callback = std::move(callback);
}

void Window::wait_resize()
{
    glfwGetFramebufferSize(m_window, &m_width, &m_height);
    while (m_width == 0 || m_height == 0)
    {
        glfwGetFramebufferSize(m_window, &m_width, &m_height);
        glfwWaitEvents();
    }
}