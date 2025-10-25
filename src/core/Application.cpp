#include "steeplejack/Application.hpp"

#include <iostream>
#include "spdlog/spdlog.h"

#include "vulkan_context_builder.h"
#include "vulkan_engine.h"

namespace steeplejack {

Application::Application(AppConfig config) : config_(std::move(config)) {}

int Application::run() {
    std::cout << "Launching " << config_.name << " (" << config_.width << "x" << config_.height
              << ")" << std::endl;

#ifdef NDEBUG
    const bool enableValidationLayers = false;
    spdlog::set_level(spdlog::level::info);
#else
    const bool enableValidationLayers = true;
    spdlog::set_level(spdlog::level::debug);
#endif

    try {
        auto context = VulkanContextBuilder()
                           .add_window(config_.width, config_.height, config_.name)
                           .add_device(enableValidationLayers)
                           .add_graphics_queue()
                           .add_adhoc_queues()
                           .add_swapchain()
                           .add_depth_buffer()
                           .add_render_pass()
                           .add_framebuffers()
                           .build();
        VulkanEngine(std::move(context)).run();
    } catch (const std::exception& e) {
        spdlog::error("Fatal: {}", e.what());
        return 1;
    }

    return 0;
}

}  // namespace steeplejack
