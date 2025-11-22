#include "graphics_queue.h"

#include <array>
#include <limits>
#include <spdlog/spdlog.h>

using namespace steeplejack;

namespace
{
[[nodiscard]] VkCommandPool create_command_pool(const Device& device)
{
    spdlog::info("Creating Graphics Command Pool");

    VkCommandPoolCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    create_info.queueFamilyIndex = device.graphics_queue_index();
    create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VkCommandPool command_pool = nullptr;
    if (vkCreateCommandPool(device.vk(), &create_info, nullptr, &command_pool) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create command pool");
    }

    return command_pool;
}

[[nodiscard]] std::vector<VkCommandBuffer> create_command_buffers(const Device& device, VkCommandPool command_pool)
{
    spdlog::info("Creating Command Buffers");

    VkCommandBufferAllocateInfo allocate_info{};
    allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocate_info.commandPool = command_pool;
    allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocate_info.commandBufferCount = Device::kMaxFramesInFlight;

    std::vector<VkCommandBuffer> command_buffers(allocate_info.commandBufferCount);
    if (vkAllocateCommandBuffers(device.vk(), &allocate_info, command_buffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate command buffers");
    }

    return command_buffers;
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

[[nodiscard]] std::vector<VkFence> create_fences(const Device& device)
{
    spdlog::info("Creating Fences");

    std::vector<VkFence> fences(Device::kMaxFramesInFlight);
    for (auto& fence : fences)
    {
        VkFenceCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        if (vkCreateFence(device.vk(), &create_info, nullptr, &fence) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create fence");
        }
    }

    return fences;
}
} // namespace

GraphicsQueue::GraphicsQueue(const Device& device) :
    m_device(device),
    m_graphics_queue(device.graphics_queue()),
    m_command_pool(create_command_pool(device)),
    m_command_buffers(create_command_buffers(device, m_command_pool)),
    m_image_available(create_semaphores(device, Device::kMaxFramesInFlight)),
    m_in_flight_fences(create_fences(device))
{
}

GraphicsQueue::~GraphicsQueue()
{
    spdlog::info("Destroying Graphics Commands");

    for (auto* fence : m_in_flight_fences)
    {
        vkDestroyFence(m_device.vk(), fence, nullptr);
    }

    for (auto* semaphore : m_image_available)
    {
        vkDestroySemaphore(m_device.vk(), semaphore, nullptr);
    }

    vkDestroyCommandPool(m_device.vk(), m_command_pool, nullptr);
}

VkFramebuffer
GraphicsQueue::prepare_framebuffer(uint32_t current_frame, const Swapchain& swapchain, const Framebuffers& framebuffers)
{
    assert(m_swapchain == nullptr);
    assert(m_render_finished_semaphore == nullptr);

    m_current_frame = current_frame;
    m_swapchain = swapchain.vk();

    vkWaitForFences(
        m_device.vk(),
        1,
        &m_in_flight_fences[m_current_frame],
        VK_TRUE,
        std::numeric_limits<uint64_t>::max());

    vkResetFences(m_device.vk(), 1, &m_in_flight_fences[m_current_frame]);

    VkResult const result = vkAcquireNextImageKHR(
        m_device.vk(),
        m_swapchain,
        std::numeric_limits<uint64_t>::max(),
        m_image_available[m_current_frame],
        nullptr,
        &m_image_index);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        m_swapchain = nullptr;
        m_render_finished_semaphore = nullptr;
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
    assert(m_swapchain != nullptr);

    if (vkResetCommandBuffer(m_command_buffers[m_current_frame], 0) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to reset command buffer");
    }

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(m_command_buffers[m_current_frame], &begin_info) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to begin recording command buffer");
    }

    return m_command_buffers[m_current_frame];
}

void GraphicsQueue::submit_command() const
{
    assert(m_swapchain != nullptr);
    assert(m_render_finished_semaphore != nullptr);

    if (vkEndCommandBuffer(m_command_buffers[m_current_frame]) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to end recording command buffer");
    }

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &m_command_buffers[m_current_frame];

    const std::array<VkSemaphore, 1> wait_semaphores{m_image_available[m_current_frame]};
    const std::array<VkPipelineStageFlags, 1> wait_stages{VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores.data();
    submit_info.pWaitDstStageMask = wait_stages.data();

    const std::array<VkSemaphore, 1> signal_semaphores{m_render_finished_semaphore};
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores.data();

    vkResetFences(m_device.vk(), 1, &m_in_flight_fences[m_current_frame]);

    if (vkQueueSubmit(m_graphics_queue, 1, &submit_info, m_in_flight_fences[m_current_frame]) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to submit draw command buffer");
    }
}

bool GraphicsQueue::present_framebuffer()
{
    assert(m_swapchain != nullptr);
    assert(m_render_finished_semaphore != nullptr);

    auto* swapchain = m_swapchain;
    auto* render_finished_semaphore = m_render_finished_semaphore;
    m_swapchain = nullptr;
    m_render_finished_semaphore = nullptr;

    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    const std::array<VkSemaphore, 1> wait_semaphores{render_finished_semaphore};
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = wait_semaphores.data();

    const std::array<VkSwapchainKHR, 1> swapchains{swapchain};
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swapchains.data();
    present_info.pImageIndices = &m_image_index;

    VkResult const result = vkQueuePresentKHR(m_graphics_queue, &present_info);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        return false;
    }
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to present swap chain image");
    }

    return true;
}
