#include "swapchain.h"

#include "spdlog/spdlog.h"

using namespace steeplejack;

namespace
{
[[nodiscard]] vkb::Swapchain create_swapchain(const Device& device)
{
    spdlog::info("Creating Swapchain");

    vkb::SwapchainBuilder swapchain_builder{device.vkb()};

    auto swapchain_ret = swapchain_builder.set_desired_present_mode(VK_PRESENT_MODE_MAILBOX_KHR).build();
    if (!swapchain_ret)
    {
        throw std::runtime_error("Failed to create swapchain: " + swapchain_ret.error().message());
    }

    auto swapchain = swapchain_ret.value();
    if (swapchain.image_count < 3)
    {
        throw std::runtime_error("Swapchain image count must be at least 3");
    }

    return swapchain;
}

[[nodiscard]] VkViewport create_viewport(VkExtent2D extent)
{
    VkViewport viewport{};
    viewport.x = 0.0F;
    viewport.y = 0.0F;
    viewport.width = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0.0F;
    viewport.maxDepth = 1.0F;

    return viewport;
}

[[nodiscard]] VkRect2D create_scissor(VkExtent2D extent)
{
    VkRect2D scissor{};
    scissor.offset = {.x = 0, .y = 0};
    scissor.extent = extent;

    return scissor;
}

[[nodiscard]] std::vector<VkSemaphore> create_semaphores(const Device& device, size_t count)
{
    spdlog::info("Creating {} Semaphores", count);

    std::vector<VkSemaphore> semaphores(count);
    for (auto& semaphore : semaphores)
    {
        VkSemaphoreCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        if (vkCreateSemaphore(device.vk(), &create_info, nullptr, &semaphore) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create semaphore");
        }
    }

    return semaphores;
}
} // namespace

Swapchain::Swapchain(const Device& device) :
    m_device(device),
    m_swapchain(create_swapchain(device)),
    m_swapchain_images(m_swapchain.get_images().value()),
    m_swapchain_image_views(m_swapchain.get_image_views().value()),
    m_render_finished(create_semaphores(device, m_swapchain.image_count)),
    m_viewport(create_viewport(m_swapchain.extent)),
    m_scissor(create_scissor(m_swapchain.extent))
{
}

Swapchain::~Swapchain()
{
    spdlog::info("Destroying Swapchain");

    for (auto* semaphore : m_render_finished)
    {
        vkDestroySemaphore(m_device.vk(), semaphore, nullptr);
    }

    for (auto* image_view : m_swapchain_image_views)
    {
        vkDestroyImageView(m_device.vk(), image_view, nullptr);
    }

    vkb::destroy_swapchain(m_swapchain);
}

VkSwapchainKHR Swapchain::vk() const
{
    return m_swapchain.swapchain;
}

VkExtent2D Swapchain::extent() const
{
    return m_swapchain.extent;
}

float Swapchain::aspect_ratio() const
{
    return static_cast<float>(m_swapchain.extent.width) / static_cast<float>(m_swapchain.extent.height);
}

uint32_t Swapchain::image_count() const
{
    return m_swapchain.image_count;
}

VkFormat Swapchain::image_format() const
{
    return m_swapchain.image_format;
}

const VkImage& Swapchain::image(size_t image_index) const
{
    return m_swapchain_images[image_index];
}

const VkImageView& Swapchain::image_view(size_t image_index) const
{
    return m_swapchain_image_views[image_index];
}

VkSemaphore Swapchain::render_finished(size_t image_index) const
{
    return m_render_finished[image_index];
}

void Swapchain::clip(VkCommandBuffer command_buffer) const
{
    vkCmdSetViewport(command_buffer, 0, 1, &m_viewport);
    vkCmdSetScissor(command_buffer, 0, 1, &m_scissor);
}
