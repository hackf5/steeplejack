#pragma once

#include <memory>
#include <vulkan/vulkan.h>

#include "util/no_copy_or_move.h"
#include "vulkan_context.h"
#include "scenes/render_scene.h"

namespace steeplejack
{
class VulkanEngine: NoCopyOrMove
{
private:
    std::unique_ptr<VulkanContext> m_context;

    uint32_t m_current_frame = 0;

    void draw_frame();
    void recreate_swapchain();
    void render(VkFramebuffer framebuffer);

    void next_frame()
    {
        m_current_frame = (m_current_frame + 1) % Device::max_frames_in_flight;
    }

public:
    VulkanEngine(std::unique_ptr<VulkanContext> context);

    void run();
};
}