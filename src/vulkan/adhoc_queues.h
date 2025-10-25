#pragma once

#include <vector>
#include <vulkan/vulkan.h>

#include "util/no_copy_or_move.h"
#include "device.h"

namespace steeplejack
{
class AdhocQueue: NoCopyOrMove
{
public:
    enum QueueFamily
    {
        graphics,
        present,
        transfer
    };

    AdhocQueue(
        const Device &device,
        QueueFamily family);

private:
    const Device &m_device;

    const VkQueue m_queue;
    const VkCommandPool m_command_pool;
    const VkCommandBuffer m_command_buffer;

    VkQueue get_queue(QueueFamily family) const;
    uint32_t get_queue_index(QueueFamily family) const;
    VkCommandPool create_command_pool(QueueFamily family);
    VkCommandBuffer create_command_buffer();

public:
    ~AdhocQueue();

    VkCommandBuffer begin() const;
    void submit_and_wait() const;
};

class AdhocQueues: NoCopyOrMove
{
private:
    const AdhocQueue m_transfer_queue;
    const AdhocQueue m_graphics_queue;
    const AdhocQueue m_present_queue;

public:
    AdhocQueues(const Device &device):
        m_transfer_queue(device, AdhocQueue::transfer),
        m_graphics_queue(device, AdhocQueue::graphics),
        m_present_queue(device, AdhocQueue::present)
    {
    }

    const AdhocQueue &transfer() const { return m_transfer_queue; }
    const AdhocQueue &graphics() const { return m_graphics_queue; }
    const AdhocQueue &present() const { return m_present_queue; }
};
}