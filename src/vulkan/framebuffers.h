#pragma once

#include "multisampler.h"

#include <vector>

namespace steeplejack
{
class Swapchain;
class RenderPass;
class DepthBuffer;
class Device;

class Framebuffers
{
  private:
    const Device& m_device;
    const Multisampler m_multisampler;

    const std::vector<VkFramebuffer> m_framebuffers;

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

    [[nodiscard]] VkFramebuffer at(uint32_t image_index) const;
};
} // namespace steeplejack
