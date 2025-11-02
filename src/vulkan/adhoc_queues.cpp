#include "adhoc_queues.h"

#include "spdlog/spdlog.h"

using namespace steeplejack;

AdhocQueue::AdhocQueue(const Device& device, QueueFamily family) :
    m_device(device),
    m_queue(get_queue(family)),
    m_command_pool(create_command_pool(family)),
    m_command_buffer(create_command_buffer())
{
}

AdhocQueue::~AdhocQueue()
{
    spdlog::info("Destroying Buffer Transfer Queue");
    vkFreeCommandBuffers(m_device, m_command_pool, 1, &m_command_buffer);
    vkDestroyCommandPool(m_device, m_command_pool, nullptr);
}

VkQueue AdhocQueue::get_queue(QueueFamily family) const
{
    switch (family)
    {
    case graphics:
        return m_device.graphics_queue();
    case present:
        return m_device.present_queue();
    case transfer:
        return m_device.transfer_queue();
    default:
        throw std::runtime_error("Invalid queue family");
    }
}

uint32_t AdhocQueue::get_queue_index(QueueFamily family) const
{
    switch (family)
    {
    case graphics:
        return m_device.graphics_queue_index();
    case present:
        return m_device.present_queue_index();
    case transfer:
        return m_device.transfer_queue_index();
    default:
        throw std::runtime_error("Invalid queue family");
    }
}

VkCommandPool AdhocQueue::create_command_pool(QueueFamily family)
{
    VkCommandPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = get_queue_index(family);
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VkCommandPool command_pool = nullptr;
    if (vkCreateCommandPool(m_device, &pool_info, nullptr, &command_pool) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create buffer transfer queue command pool");
    }

    return command_pool;
}

VkCommandBuffer AdhocQueue::create_command_buffer()
{
    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = m_command_pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = 1;

    VkCommandBuffer command_buffer = nullptr;
    if (vkAllocateCommandBuffers(m_device, &alloc_info, &command_buffer) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate command buffers");
    }

    return command_buffer;
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