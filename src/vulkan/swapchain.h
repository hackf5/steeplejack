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

    [[nodiscard]] VkSwapchainKHR vk() const;
    [[nodiscard]] VkExtent2D extent() const;
    [[nodiscard]] float aspect_ratio() const;
    [[nodiscard]] uint32_t image_count() const;
    [[nodiscard]] VkFormat image_format() const;
    [[nodiscard]] const VkImage& image(size_t image_index) const;
    [[nodiscard]] const VkImageView& image_view(size_t image_index) const;
    [[nodiscard]] VkSemaphore render_finished(size_t image_index) const;

    void clip(VkCommandBuffer command_buffer) const;
};
} // namespace steeplejack
