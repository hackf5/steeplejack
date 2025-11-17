#pragma once

#include "scenes/render_scene.h"
#include "vulkan_context.h"

#include <memory>
#include <vulkan/vulkan.h>

namespace steeplejack
{
class VulkanEngine
{
  private:
    std::unique_ptr<VulkanContext> m_context;

    uint32_t m_current_frame = 0;

    void draw_frame();
    void recreate_swapchain();
    void render(VkFramebuffer framebuffer);

    void next_frame()
    {
        m_current_frame = (m_current_frame + 1) % Device::kMaxFramesInFlight;
    }

  public:
    VulkanEngine(std::unique_ptr<VulkanContext> context);

    VulkanEngine(const VulkanEngine&) = delete;
    VulkanEngine& operator=(const VulkanEngine&) = delete;
    VulkanEngine(VulkanEngine&&) = delete;
    VulkanEngine& operator=(VulkanEngine&&) = delete;

    void run();
};
} // namespace steeplejack
