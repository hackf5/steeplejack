#pragma once

#include "device.h"

#include <vector>

namespace steeplejack
{
class Swapchain
{
  private:
    const Device& m_device;

    vkb::Swapchain m_swapchain;
    const std::vector<VkImage> m_swapchain_images;
    const std::vector<VkImageView> m_swapchain_image_views;
    const std::vector<VkSemaphore> m_render_finished;
    const VkViewport m_viewport;
    const VkRect2D m_scissor;

  public:
    explicit Swapchain(const Device& device);
    ~Swapchain();

    Swapchain(const Swapchain&) = delete;
    Swapchain& operator=(const Swapchain&) = delete;
    Swapchain(Swapchain&&) = delete;
    Swapchain& operator=(Swapchain&&) = delete;

    [[nodiscard]] VkSwapchainKHR vk() const
    {
        return m_swapchain.swapchain;
    }

    [[nodiscard]] VkExtent2D extent() const
    {
        return m_swapchain.extent;
    }

    [[nodiscard]] float aspect_ratio() const
    {
        return static_cast<float>(m_swapchain.extent.width) / static_cast<float>(m_swapchain.extent.height);
    }

    [[nodiscard]] uint32_t image_count() const
    {
        return m_swapchain.image_count;
    }

    [[nodiscard]] VkFormat image_format() const
    {
        return m_swapchain.image_format;
    }

    [[nodiscard]] const VkViewport& viewport() const
    {
        return m_viewport;
    }

    [[nodiscard]] const VkRect2D& scissor() const
    {
        return m_scissor;
    }

    [[nodiscard]] const VkImage& image(size_t image_index) const
    {
        return m_swapchain_images[image_index];
    }

    [[nodiscard]] const VkImageView& image_view(size_t image_index) const
    {
        return m_swapchain_image_views[image_index];
    }

    [[nodiscard]] VkSemaphore render_finished(size_t image_index) const
    {
        return m_render_finished[image_index];
    }

    void clip(VkCommandBuffer command_buffer) const
    {
        vkCmdSetViewport(command_buffer, 0, 1, &m_viewport);
        vkCmdSetScissor(command_buffer, 0, 1, &m_scissor);
    }
};
} // namespace steeplejack
