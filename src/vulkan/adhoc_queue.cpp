#include "adhoc_queue.h"

#include "spdlog/spdlog.h"
#include "vulkan/device.h"

#include <stdexcept>

using namespace steeplejack;

namespace
{
[[nodiscard]] VkQueue get_queue(const Device& device, AdhocQueueFamily family)
{
    switch (family)
    {
    case AdhocQueueFamily::Graphics:
        return device.graphics_queue();
    case AdhocQueueFamily::Present:
        return device.present_queue();
    case AdhocQueueFamily::Transfer:
        return device.transfer_queue();
    default:
        throw std::runtime_error("Invalid queue family");
    }
}

[[nodiscard]] uint32_t get_queue_index(const Device& device, AdhocQueueFamily family)
{
    switch (family)
    {
    case AdhocQueueFamily::Graphics:
        return device.graphics_queue_index();
    case AdhocQueueFamily::Present:
        return device.present_queue_index();
    case AdhocQueueFamily::Transfer:
        return device.transfer_queue_index();
    default:
        throw std::runtime_error("Invalid queue family");
    }
}

VkCommandPool create_command_pool(const Device& device, AdhocQueueFamily family)
{
    VkCommandPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = get_queue_index(device, family);
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VkCommandPool command_pool = nullptr;
    if (vkCreateCommandPool(device.vk(), &pool_info, nullptr, &command_pool) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create buffer transfer queue command pool");
    }

    return command_pool;
}

VkCommandBuffer create_command_buffer(const Device& device, VkCommandPool command_pool)
{
    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = command_pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = 1;

    VkCommandBuffer command_buffer = nullptr;
    if (vkAllocateCommandBuffers(device.vk(), &alloc_info, &command_buffer) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate command buffers");
    }

    return command_buffer;
}
} // namespace

AdhocQueue::AdhocQueue(const Device& device, AdhocQueueFamily family) :
    m_device(device),
    m_queue(get_queue(device, family)),
    m_command_pool(create_command_pool(device, family)),
    m_command_buffer(create_command_buffer(device, m_command_pool))
{
}

AdhocQueue::~AdhocQueue()
{
    spdlog::info("Destroying Buffer Transfer Queue");
    vkFreeCommandBuffers(m_device.vk(), m_command_pool, 1, &m_command_buffer);
    vkDestroyCommandPool(m_device.vk(), m_command_pool, nullptr);
}

VkCommandBuffer AdhocQueue::begin() const
{
    if (vkResetCommandBuffer(m_command_buffer, 0) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to reset transfer command buffer");
    }

    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(m_command_buffer, &begin_info) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to begin recording transfer command buffer");
    }

    return m_command_buffer;
}

void AdhocQueue::submit_and_wait() const
{
    auto* command_buffer = m_command_buffer;

    if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to record transfer command buffer");
    }

    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;

    if (vkQueueSubmit(m_queue, 1, &submit_info, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to submit transfer command buffer");
    }

    if (vkQueueWaitIdle(m_queue) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to wait for queue to become idle");
    }
}
