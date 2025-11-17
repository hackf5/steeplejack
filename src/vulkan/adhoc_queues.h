#pragma once

#include "device.h"
#include "util/no_copy_or_move.h"

#include <cstdint>
#include <vulkan/vulkan.h>

namespace steeplejack
{
class AdhocQueue : NoCopyOrMove
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

    [[nodiscard]] VkCommandBuffer begin() const;
    void submit_and_wait() const;
};

class AdhocQueues : NoCopyOrMove
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