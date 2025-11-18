#pragma once

#include "device.h"
#include "framebuffers.h"
#include "swapchain.h"

#include <vector>

namespace steeplejack
{
class GraphicsQueue
{
  private:
    const Device& m_device;

    const VkQueue m_graphics_queue;
    const VkCommandPool m_command_pool;

    const std::vector<VkCommandBuffer> m_command_buffers;
    const std::vector<VkSemaphore> m_image_available; // per-frame
    const std::vector<VkFence> m_in_flight_fences;

    VkCommandPool create_command_pool();
    std::vector<VkCommandBuffer> create_command_buffers();
    std::vector<VkSemaphore> create_semaphores(size_t count);
    std::vector<VkFence> create_fences();

    uint32_t m_image_index = 0;
    uint32_t m_current_frame = 0;
    VkSwapchainKHR m_swapchain = nullptr;
    VkSemaphore m_render_finished_semaphore = nullptr;

  public:
    explicit GraphicsQueue(const Device& device);
    ~GraphicsQueue();

    GraphicsQueue(const GraphicsQueue&) = delete;
    GraphicsQueue& operator=(const GraphicsQueue&) = delete;
    GraphicsQueue(GraphicsQueue&&) = delete;
    GraphicsQueue& operator=(GraphicsQueue&&) = delete;

    VkFramebuffer
    prepare_framebuffer(uint32_t current_frame, const Swapchain& swapchain, const Framebuffers& framebuffers);

    [[nodiscard]] VkCommandBuffer begin_command() const;
    void submit_command() const;
    bool present_framebuffer();
};
} // namespace steeplejack
