#pragma once

#include "util/no_copy_or_move.h"
#include "vulkan/depth_buffer.h"
#include "vulkan/device.h"
#include "vulkan/swapchain.h"

namespace steeplejack {

class RenderPass : NoCopyOrMove {
private:
    const Device& device_;
    const Swapchain& swapchain_;
    const VkRenderPass render_pass_;

    VkRenderPass create_render_pass(const DepthBuffer& depth_buffer) const;

public:
    RenderPass(const Device& device, const Swapchain& swapchain, const DepthBuffer& depth_buffer);
    ~RenderPass();

    operator VkRenderPass() const { return render_pass_; }
    void begin(VkCommandBuffer command_buffer, VkFramebuffer framebuffer) const;
    void end(VkCommandBuffer command_buffer) const { vkCmdEndRenderPass(command_buffer); }
};

} // namespace steeplejack

