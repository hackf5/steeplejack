#include "game_engine.h"

#include "renderer/contexts.h"
#include "renderer/scene_runner.h"
#include "scenes/minimal_scene_runner.h"
#include "vulkan/device.h"
#include "vulkan/graphics_queue.h"
#include "vulkan/swapchain.h"
#include "vulkan/window.h"

#include <chrono>
#include <memory>
#include <stdexcept>

using namespace steeplejack;
using namespace steeplejack::renderer;

GameEngine::GameEngine(std::unique_ptr<Backend> backend) :
    m_backend(std::move(backend)),
    m_scene(std::make_unique<MinimalSceneRunner>(*m_backend)),
    m_last_time(std::chrono::system_clock::now())
{
}

void GameEngine::run()
{
    if (m_scene == nullptr)
    {
        throw std::runtime_error("GameEngine: scene runner is not set");
    }

    m_scene->load_resources();
    m_scene->build_scene_backend();

    while (!m_backend->window().should_close())
    {
        m_backend->window().poll_events();

        auto frame_ctx = next_frame();
        m_scene->update(frame_ctx);

        auto* framebuffer = m_backend->graphics_queue().prepare_framebuffer(
            frame_ctx.frame_index,
            m_backend->swapchain(),
            m_backend->framebuffers());
        if (framebuffer == nullptr)
        {
            rebuild_swapchain();
            continue;
        }

        VkCommandBuffer command_buffer = m_backend->graphics_queue().begin_command();

        RenderContext render_ctx{
            .command_buffer = command_buffer,
            .framebuffer = framebuffer,
            .frame_index = frame_ctx.frame_index,
        };

        m_scene->render(render_ctx);

        m_backend->graphics_queue().submit_command();

        if (!m_backend->graphics_queue().present_framebuffer())
        {
            rebuild_swapchain();
            continue;
        }
    }
}

renderer::FrameContext GameEngine::next_frame()
{
    auto now = std::chrono::system_clock::now();
    float dt = std::chrono::duration<float, std::chrono::seconds::period>(now - m_last_time).count();
    m_last_time = now;

    renderer::FrameContext frame_ctx{
        .frame_index = m_frame_index,
        .aspect_ratio = m_backend->swapchain().aspect_ratio(),
        .delta_seconds = dt,
    };

    m_frame_index = (m_frame_index + 1) % Device::kMaxFramesInFlight;

    return frame_ctx;
}

void GameEngine::rebuild_swapchain()
{
    m_backend->rebuild_swapchain();
    m_scene->build_scene_backend();
    m_frame_index = 0;
}