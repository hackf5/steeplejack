#include "application.h"

#include "scenes/woxel_column.h"
#include "spdlog/spdlog.h"
#include "vulkan_context_builder.h"
#include "vulkan_engine.h"

#include <iostream>

namespace
{
constexpr int kWindowWidth = 1280;
constexpr int kWindowHeight = 720;
} // namespace

namespace steeplejack
{

Application::Application() = default;

int Application::run()
{
#ifdef NDEBUG
    const bool enableValidationLayers = false;
    spdlog::set_level(spdlog::level::info);
#else
    const bool enable_validation_layers = true;
    spdlog::set_level(spdlog::level::debug);
#endif

    try
    {
        using namespace steeplejack;

        auto layout_builder = [](DescriptorSetLayoutBuilder& builder)
        {
            builder
                .add_uniform_buffer()          // camera
                .add_uniform_buffer()          // model
                .add_combined_image_sampler(); // texture
        };

        auto scene_factory = [](const Device& device)
        {
            using namespace steeplejack::woxel;
            WoxelGrid grid{};
            grid.num_sectors = 48;
            grid.dy = 0.25f;
            grid.dz = 0.2f;

            ColumnSpec spec{};
            spec.inner_radius = 2.5f;
            spec.layers = 24;

            return std::make_unique<WoxelColumn>(device, grid, spec);
        };

        auto context = VulkanContextBuilder()
                           .add_window(kWindowWidth, kWindowHeight, "Steeplejack")
                           .add_device(enable_validation_layers)
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
    }
    catch (const std::exception& e)
    {
        spdlog::error("Fatal: {}", e.what());
        return 1;
    }

    return 0;
}

} // namespace steeplejack
