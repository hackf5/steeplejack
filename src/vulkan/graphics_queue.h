#pragma once

#include <vector>

#include "util/no_copy_or_move.h"
#include "vulkan/device.h"
#include "vulkan/swapchain.h"
#include "vulkan/framebuffers.h"

namespace steeplejack {

class GraphicsQueue : NoCopyOrMove {
private:
    const Device& device_;
    const VkQueue graphics_queue_;
    const VkCommandPool command_pool_;
    const std::vector<VkCommandBuffer> command_buffers_;
    const std::vector<VkSemaphore> image_available_;
    const std::vector<VkSemaphore> render_finished_;
    const std::vector<VkFence> in_flight_fences_;

    VkCommandPool create_command_pool();
    std::vector<VkCommandBuffer> create_command_buffers();
    std::vector<VkSemaphore> create_semaphores();
    std::vector<VkFence> create_fences();

    uint32_t image_index_ = 0;
    uint32_t current_frame_ = 0;
    VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;

public:
    explicit GraphicsQueue(const Device& device);
    ~GraphicsQueue();

    VkFramebuffer prepare_framebuffer(uint32_t current_frame,
                                      const Swapchain& swapchain,
                                      const Framebuffers& framebuffers);

    VkCommandBuffer begin_command() const;
    void submit_command() const;
    bool present_framebuffer();
};

} // namespace steeplejack

