#pragma once

#include <memory>

#include "util/no_copy_or_move.h"

#include "vulkan/adhoc_queues.h"
#include "vulkan/depth_buffer.h"
#include "vulkan/device.h"
#include "vulkan/framebuffers.h"
#include "vulkan/graphics_queue.h"
#include "vulkan/render_pass.h"
#include "vulkan/swapchain.h"
#include "vulkan/window.h"

namespace steeplejack {

class VulkanContext : NoCopyOrMove {
private:
    std::unique_ptr<Window> window_;
    std::unique_ptr<Device> device_;
    std::unique_ptr<AdhocQueues> adhoc_queues_;
    std::unique_ptr<GraphicsQueue> graphics_queue_;
    std::unique_ptr<Swapchain> swapchain_;
    std::unique_ptr<DepthBuffer> depth_buffer_;
    std::unique_ptr<RenderPass> render_pass_;
    std::unique_ptr<Framebuffers> framebuffers_;

public:
    VulkanContext() = default;

    Window& window() { return *window_; }
    const Device& device() const { return *device_; }
    const AdhocQueues& adhoc_queues() const { return *adhoc_queues_; }
    GraphicsQueue& graphics_queue() { return *graphics_queue_; }
    const GraphicsQueue& graphics_queue() const { return *graphics_queue_; }
    const Swapchain& swapchain() const { return *swapchain_; }
    const RenderPass& render_pass() const { return *render_pass_; }
    const Framebuffers& framebuffers() const { return *framebuffers_; }

    friend class VulkanContextBuilder;
};

} // namespace steeplejack

