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
    const Device* m_device;
    const AdhocQueues* m_adhoc_queues;

  public:
    BufferGPU(const Device& device, const AdhocQueues& adhoc_queues, VkDeviceSize size, VkBufferUsageFlags usage);

    BufferGPU(const BufferGPU&) = delete;
    BufferGPU& operator=(const BufferGPU&) = delete;
    BufferGPU(BufferGPU&&) noexcept = default;
    BufferGPU& operator=(BufferGPU&&) = delete;
    ~BufferGPU() = default;

    void copy_from(const BufferHost& buffer) const;

    template <typename TIter> void copy_from(TIter begin, TIter end) const
    {
        StagingBuffer staging_buffer(*m_device, begin, end);
        copy_from(staging_buffer);
    }

    void copy_from(const std::ranges::contiguous_range auto& range) const
    {
        StagingBuffer staging_buffer(*m_device, range);
        copy_from(staging_buffer);
    }
};
} // namespace steeplejack
