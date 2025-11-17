#pragma once

#include "device.h"

#include <cstdint>
#include <vulkan/vulkan.h>

namespace steeplejack
{
class AdhocQueue
{
  public:
    enum QueueFamily : std::uint8_t
    {
        graphics,
        present,
        transfer
    };

    AdhocQueue(const Device& device, QueueFamily family);

  private:
    const Device& m_device;

    const VkQueue m_queue;
    const VkCommandPool m_command_pool;
    const VkCommandBuffer m_command_buffer;

    [[nodiscard]] VkQueue get_queue(QueueFamily family) const;
    [[nodiscard]] uint32_t get_queue_index(QueueFamily family) const;
    VkCommandPool create_command_pool(QueueFamily family);
    VkCommandBuffer create_command_buffer();

  public:
    ~AdhocQueue();

    AdhocQueue(const AdhocQueue&) = delete;
    AdhocQueue& operator=(const AdhocQueue&) = delete;
    AdhocQueue(AdhocQueue&&) = delete;
    AdhocQueue& operator=(AdhocQueue&&) = delete;

    [[nodiscard]] VkCommandBuffer begin() const;
    void submit_and_wait() const;
};

class AdhocQueues
{
  private:
    const AdhocQueue m_transfer_queue;
    const AdhocQueue m_graphics_queue;
    const AdhocQueue m_present_queue;

  public:
    AdhocQueues(const Device& device) :
        m_transfer_queue(device, AdhocQueue::transfer),
        m_graphics_queue(device, AdhocQueue::graphics),
        m_present_queue(device, AdhocQueue::present)
    {
    }

    AdhocQueues(const AdhocQueues&) = delete;
    AdhocQueues& operator=(const AdhocQueues&) = delete;
    AdhocQueues(AdhocQueues&&) = delete;
    AdhocQueues& operator=(AdhocQueues&&) = delete;

    [[nodiscard]] const AdhocQueue& transfer() const
    {
        return m_transfer_queue;
    }
    [[nodiscard]] const AdhocQueue& graphics() const
    {
        return m_graphics_queue;
    }
    [[nodiscard]] const AdhocQueue& present() const
    {
        return m_present_queue;
    }
};
} // namespace steeplejack
