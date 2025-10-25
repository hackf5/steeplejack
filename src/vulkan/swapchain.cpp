#include "swapchain.h"

#include "spdlog/spdlog.h"

using namespace steeplejack;

Swapchain::Swapchain(const Device &device):
    m_device(device),
    m_swapchain(create_swapchain()),
    m_swapchain_images(m_swapchain.get_images().value()),
    m_swapchain_image_views(m_swapchain.get_image_views().value()),
    m_render_finished(create_semaphores(m_swapchain.image_count)),
    m_viewport(create_viewport()),
    m_scissor(create_scissor())
{
}

Swapchain::~Swapchain()
{
    spdlog::info("Destroying Swapchain");

    for (auto semaphore : m_render_finished)
    {
        vkDestroySemaphore(m_device, semaphore, nullptr);
    }

    for (auto image_view : m_swapchain_image_views)
    {
        vkDestroyImageView(m_device, image_view, nullptr);
    }

    vkb::destroy_swapchain(m_swapchain);
}

vkb::Swapchain Swapchain::create_swapchain()
{
    spdlog::info("Creating Swapchain");

    vkb::SwapchainBuilder swapchain_builder { m_device };

    auto swapchain_ret = swapchain_builder
        .set_desired_present_mode(VK_PRESENT_MODE_MAILBOX_KHR)
        .build();
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

VkViewport Swapchain::create_viewport()
{
    VkViewport viewport {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_swapchain.extent.width);
    viewport.height = static_cast<float>(m_swapchain.extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    return viewport;
}

VkRect2D Swapchain::create_scissor()
{
    VkRect2D scissor {};
    scissor.offset = { 0, 0 };
    scissor.extent = m_swapchain.extent;

    return scissor;
}

std::vector<VkSemaphore> Swapchain::create_semaphores(size_t count)
{
    spdlog::info("Creating {} Semaphores", count);

    std::vector<VkSemaphore> semaphores(count);
    for (auto &semaphore : semaphores)
    {
        VkSemaphoreCreateInfo create_info {};
        create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        if (vkCreateSemaphore(m_device, &create_info, nullptr, &semaphore) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create semaphore");
        }
    }

    return semaphores;
}