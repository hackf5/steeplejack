#include "steeplejack/Application.hpp"

#include <iostream>
#include "spdlog/spdlog.h"

#include "vulkan_context_builder.h"
#include "vulkan_engine.h"
#include "scenes/cubes_one.h"

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
        using namespace levin;

        auto layout_builder = [](DescriptorSetLayoutBuilder &builder) {
            builder
                .add_uniform_buffer() // camera
                .add_uniform_buffer() // model
                .add_combined_image_sampler(); // texture
        };

        auto scene_factory = [](const Device &device) { return std::make_unique<CubesOne>(device); };

        auto context = VulkanContextBuilder()
                           .add_window(config_.width, config_.height, config_.name)
                           .add_device(enableValidationLayers)
                           .add_graphics_queue()
                           .add_adhoc_queues()
                           .add_graphics_buffers()
                           .add_descriptor_set_layout(layout_builder)
                           .add_sampler()
                           .add_texture_factory()
                           .add_scene(scene_factory)
                           .add_swapchain()
                           .add_depth_buffer()
                           .add_render_pass()
                           .add_framebuffers()
                           .add_graphics_pipeline()
                           .add_gui()
                           .build();

        VulkanEngine(std::move(context)).run();
    } catch (const std::exception& e) {
        spdlog::error("Fatal: {}", e.what());
        return 1;
    }

    return 0;
}

}  // namespace steeplejack
