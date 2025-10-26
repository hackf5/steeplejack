#pragma once

#include "buffer.h"
#include "staging_buffer.h"
#include "vulkan/adhoc_queues.h"

#include <ranges>
#include <vulkan/vulkan.h>

namespace steeplejack
{
class BufferGPU : public Buffer
{
  private:
    const Device& m_device;
    const AdhocQueues& m_adhoc_queues;

  public:
    BufferGPU(const Device& device, const AdhocQueues& adhoc_queues, VkDeviceSize size, VkBufferUsageFlags usage) :
        Buffer(device, size, usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT),
        m_device(device),
        m_adhoc_queues(adhoc_queues) {};

    void copy_from(const BufferHost& buffer) const
    {
        auto command_buffer = m_adhoc_queues.transfer().begin();

        VkBufferCopy copy_region = {};
        copy_region.size = buffer.size();
        vkCmdCopyBuffer(command_buffer, buffer, *this, 1, &copy_region);

        m_adhoc_queues.transfer().submit_and_wait();
    }

    template <typename TIter> void copy_from(TIter begin, TIter end) const
    {
        StagingBuffer staging_buffer(m_device, begin, end);
        copy_from(staging_buffer);
    }

    void copy_from(const std::ranges::contiguous_range auto& range) const
    {
        StagingBuffer staging_buffer(m_device, range);
        copy_from(staging_buffer);
    }
};
} // namespace steeplejack
