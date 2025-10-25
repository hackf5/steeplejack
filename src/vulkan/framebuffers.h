#pragma once

#include <vector>

#include "util/no_copy_or_move.h"
#include "vulkan/depth_buffer.h"
#include "vulkan/device.h"
#include "vulkan/multisampler.h"
#include "vulkan/render_pass.h"
#include "vulkan/swapchain.h"

namespace steeplejack {

class Framebuffers : NoCopyOrMove {
private:
    const Device& device_;
    const Multisampler multisampler_;
    const std::vector<VkFramebuffer> framebuffers_;

    std::vector<VkFramebuffer> create_framebuffers(const Swapchain& swapchain,
                                                   const RenderPass& render_pass,
                                                   const DepthBuffer& depth_buffer);

public:
    Framebuffers(const Device& device,
                 const Swapchain& swapchain,
                 const RenderPass& render_pass,
                 const DepthBuffer& depth_buffer);
    ~Framebuffers();

    VkFramebuffer get(uint32_t image_index) const { return framebuffers_[image_index]; }
};

} // namespace steeplejack

