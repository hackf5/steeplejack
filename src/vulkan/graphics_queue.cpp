#include "graphics_queue.h"

#include <limits>

#include <spdlog/spdlog.h>

using namespace steeplejack;

GraphicsQueue::GraphicsQueue(const Device &device):
    m_device(device),
    m_graphics_queue(device.graphics_queue()),
    m_command_pool(create_command_pool()),
    m_command_buffers(create_command_buffers()),
    m_image_available(create_semaphores(Device::max_frames_in_flight)),
    m_in_flight_fences(create_fences())
{
}

GraphicsQueue::~GraphicsQueue()
{
    spdlog::info("Destroying Graphics Commands");

    for (auto fence : m_in_flight_fences)
    {
        vkDestroyFence(m_device, fence, nullptr);
    }

    for (auto semaphore : m_image_available)
    {
        vkDestroySemaphore(m_device, semaphore, nullptr);
    }

    vkDestroyCommandPool(m_device, m_command_pool, nullptr);
}

VkCommandPool GraphicsQueue::create_command_pool()
{
    spdlog::info("Creating Graphics Command Pool");

    VkCommandPoolCreateInfo create_info {};
    create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    create_info.queueFamilyIndex = m_device.graphics_queue_index();
    create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VkCommandPool command_pool;
    if (vkCreateCommandPool(m_device, &create_info, nullptr, &command_pool) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create command pool");
    }

    return command_pool;
}

std::vector<VkCommandBuffer> GraphicsQueue::create_command_buffers()
{
    spdlog::info("Creating Command Buffers");

    VkCommandBufferAllocateInfo allocate_info {};
    allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocate_info.commandPool = m_command_pool;
    allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocate_info.commandBufferCount = Device::max_frames_in_flight;

    std::vector<VkCommandBuffer> command_buffers(allocate_info.commandBufferCount);
    if (vkAllocateCommandBuffers(m_device, &allocate_info, command_buffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate command buffers");
    }

    return command_buffers;
}

std::vector<VkSemaphore> GraphicsQueue::create_semaphores(size_t count)
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

std::vector<VkFence> GraphicsQueue::create_fences()
{
    spdlog::info("Creating Fences");

    std::vector<VkFence> fences(Device::max_frames_in_flight);
    for (auto &fence : fences)
    {
        VkFenceCreateInfo create_info {};
        create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        if (vkCreateFence(m_device, &create_info, nullptr, &fence) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create fence");
        }
    }

    return fences;
}

VkFramebuffer GraphicsQueue::prepare_framebuffer(
    uint32_t current_frame,
    const Swapchain &swapchain,
    const Framebuffers &framebuffers)
{
    assert(m_swapchain == VK_NULL_HANDLE);
    assert(m_render_finished_semaphore == VK_NULL_HANDLE);

    m_current_frame = current_frame;
    m_swapchain = swapchain;

    vkWaitForFences(
        m_device,
        1,
        &m_in_flight_fences[m_current_frame],
        VK_TRUE,
        std::numeric_limits<uint64_t>::max());

    vkResetFences(m_device, 1, &m_in_flight_fences[m_current_frame]);

    VkResult result = vkAcquireNextImageKHR(
        m_device,
        swapchain,
        std::numeric_limits<uint64_t>::max(),
        m_image_available[m_current_frame],
        VK_NULL_HANDLE,
        &m_image_index);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        return nullptr;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("Failed to acquire swap chain image");
    }

    m_render_finished_semaphore = swapchain.render_finished(m_image_index);

    return framebuffers.get(m_image_index);
}

VkCommandBuffer GraphicsQueue::begin_command() const
{
    assert(m_swapchain != VK_NULL_HANDLE);

    if (vkResetCommandBuffer(m_command_buffers[m_current_frame], 0) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to reset command buffer");
    }

    VkCommandBufferBeginInfo begin_info {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(m_command_buffers[m_current_frame], &begin_info) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to begin recording command buffer");
    }

    return m_command_buffers[m_current_frame];
}

void GraphicsQueue::submit_command() const
{
    assert(m_swapchain != VK_NULL_HANDLE);

    if (vkEndCommandBuffer(m_command_buffers[m_current_frame]) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to end recording command buffer");
    }

    VkSubmitInfo submit_info {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &m_command_buffers[m_current_frame];

    VkSemaphore wait_semaphores[] = { m_image_available[m_current_frame] };
    VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;

    VkSemaphore signal_semaphores[] = { m_render_finished_semaphore };
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    vkResetFences(m_device, 1, &m_in_flight_fences[m_current_frame]);

    if (vkQueueSubmit(
        m_graphics_queue,
        1,
        &submit_info,
        m_in_flight_fences[m_current_frame]) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to submit draw command buffer");
    }
}

bool GraphicsQueue::present_framebuffer()
{
    assert(m_swapchain != VK_NULL_HANDLE);
    auto swapchain = m_swapchain;
    auto render_finished_semaphore = m_render_finished_semaphore;
    m_swapchain = VK_NULL_HANDLE;
    m_render_finished_semaphore = VK_NULL_HANDLE;

    VkPresentInfoKHR present_info {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    VkSemaphore wait_semaphores[] = { render_finished_semaphore };
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = wait_semaphores;

    VkSwapchainKHR swapchains[] = { swapchain };
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swapchains;
    present_info.pImageIndices = &m_image_index;

    VkResult result = vkQueuePresentKHR(m_graphics_queue, &present_info);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        return false;
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to present swap chain image");
    }

    return true;
}