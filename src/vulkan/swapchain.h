#pragma once

#include <vector>

#include "util/no_copy_or_move.h"
#include "device.h"

namespace steeplejack
{
class Swapchain: NoCopyOrMove
{
private:
    const Device &m_device;

    vkb::Swapchain m_swapchain;
    const std::vector<VkImage> m_swapchain_images;
    const std::vector<VkImageView> m_swapchain_image_views;
    const std::vector<VkSemaphore> m_render_finished;
    const VkViewport m_viewport;
    const VkRect2D m_scissor;

    vkb::Swapchain create_swapchain();
    VkViewport create_viewport();
    VkRect2D create_scissor();
    std::vector<VkSemaphore> create_semaphores(size_t count);

public:
    Swapchain(const Device &device);
    ~Swapchain();

    operator VkSwapchainKHR() const { return m_swapchain.swapchain; }

    VkExtent2D extent() const { return m_swapchain.extent; }

    float aspect_ratio() const
    {
        return static_cast<float>(m_swapchain.extent.width) /
            static_cast<float>(m_swapchain.extent.height);
    }

    uint32_t image_count() const { return m_swapchain.image_count; }

    VkFormat image_format() const { return m_swapchain.image_format; }

    const VkViewport &viewport() const { return m_viewport; }

    const VkRect2D &scissor() const { return m_scissor; }

    const VkImage &image(size_t image_index) const { return m_swapchain_images[image_index]; }

    const VkImageView &image_view(size_t image_index) const { return m_swapchain_image_views[image_index]; }

    VkSemaphore render_finished(size_t image_index) const { return m_render_finished[image_index]; }

    void clip(VkCommandBuffer command_buffer) const
    {
        vkCmdSetViewport(command_buffer, 0, 1, &m_viewport);
        vkCmdSetScissor(command_buffer, 0, 1, &m_scissor);
    }
};
}