#include "application.h"

#include "scenes/cubes_one.h"
#include "spdlog/spdlog.h"
#include "util/runtime_paths.h"
#include "vulkan_context_builder.h"
#include "vulkan_engine.h"

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
        // Ensure relative asset paths (e.g., shaders/) resolve regardless of launch CWD
        set_working_directory_to_executable();

        auto scene_factory = [](const Device& device) { return std::make_unique<CubesOne>(device); };

        auto context = VulkanContextBuilder()
                           .add_window(kWindowWidth, kWindowHeight, "Steeplejack")
                           .add_device(enable_validation_layers)
                           .add_graphics_queue()
                           .add_adhoc_queues()
                           .add_graphics_buffers()
                           .add_descriptor_set_layout("default")
                           .add_descriptor_set_layout("shadow")
                           .add_sampler()
                           .add_shadow_sampler()
                           .add_texture_factory()
                           .add_material_factory()
                           .add_shadow_mapping()
                           .add_shadow_render_pass()
                           .add_shadow_framebuffers()
                           .add_shadow_pipeline()
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
