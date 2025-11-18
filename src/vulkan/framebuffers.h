#pragma once

#include "depth_buffer.h"
#include "device.h"
#include "multisampler.h"
#include "render_pass.h"
#include "swapchain.h"

#include <vector>

namespace steeplejack
{
class Framebuffers
{
  private:
    const Device& m_device;
    const Multisampler m_multisampler;

    const std::vector<VkFramebuffer> m_framebuffers;

    std::vector<VkFramebuffer>
    create_framebuffers(const Swapchain& swapchain, const RenderPass& render_pass, const DepthBuffer& depth_buffer);

  public:
    Framebuffers(
        const Device& device,
        const Swapchain& swapchain,
        const RenderPass& render_pass,
        const DepthBuffer& depth_buffer);
    ~Framebuffers();

    Framebuffers(const Framebuffers&) = delete;
    Framebuffers& operator=(const Framebuffers&) = delete;
    Framebuffers(Framebuffers&&) = delete;
    Framebuffers& operator=(Framebuffers&&) = delete;

    [[nodiscard]] VkFramebuffer get(uint32_t image_index) const
    {
        return m_framebuffers[image_index];
    }
};
} // namespace steeplejack
