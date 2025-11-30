#include "game_engine.h"

#include "renderer/contexts.h"
#include "renderer/scene_runner.h"
#include "vulkan/device.h"
#include "vulkan/graphics_queue.h"
#include "vulkan/swapchain.h"
#include "vulkan/window.h"

#include <chrono>
#include <stdexcept>

using namespace steeplejack;
using namespace steeplejack::renderer;

GameEngine::GameEngine(std::unique_ptr<Backend> backend) : m_backend(std::move(backend)) {}

void GameEngine::run()
{
    if (m_scene == nullptr)
    {
        throw std::runtime_error("GameEngine: scene runner is not set");
    }

    m_scene->load_resources();
    m_scene->build_scene_backend();

    uint32_t frame_index = 0;
    auto last_time = std::chrono::high_resolution_clock::now();

    while (!m_backend->window().should_close())
    {
        m_backend->window().poll_events();

        auto now = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::seconds::period>(now - last_time).count();
        last_time = now;

        FrameContext frame_ctx{
            .frame_index = frame_index,
            .aspect_ratio = m_backend->swapchain().aspect_ratio(),
            .delta_seconds = dt,
        };
        m_scene->update(frame_ctx);

        auto* framebuffer = m_backend->graphics_queue().prepare_framebuffer(
            frame_index,
            m_backend->swapchain(),
            m_backend->framebuffers());
        if (framebuffer == nullptr)
        {
            // Swapchain out of date; defer recreation wiring for now.
            break;
        }

        VkCommandBuffer command_buffer = m_backend->graphics_queue().begin_command();

        RenderContext render_ctx{
            .command_buffer = command_buffer,
            .framebuffer = framebuffer,
            .frame_index = frame_index,
        };

        m_scene->render(render_ctx);

        m_backend->graphics_queue().submit_command();

        if (!m_backend->graphics_queue().present_framebuffer())
        {
            // Swapchain out of date; defer recreation wiring for now.
            break;
        }

        frame_index = (frame_index + 1) % Device::kMaxFramesInFlight;
    }
}
