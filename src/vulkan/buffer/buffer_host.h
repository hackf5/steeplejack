#pragma once

#include <ranges>

#include <vulkan/vulkan.h>

#include "util/memory.h"
#include "buffer.h"

namespace levin
{
class BufferHost: public Buffer
{
private:
    void copy_from(void *data, VkDeviceSize size)
    {
        memcpy(m_allocation_info.info.pMappedData, data, size);
    }

public:
    BufferHost(
        const Device &device,
        VkDeviceSize size,
        VkBufferUsageFlags usage):
        Buffer(
            device,
            size,
            usage,
            VMA_MEMORY_USAGE_AUTO,
            VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
            | VMA_ALLOCATION_CREATE_MAPPED_BIT)
    {
    }

    template <typename T>
    void copy_from(const T &data)
    {
        copy_to(m_allocation_info.info.pMappedData, data);
    }

    template <typename TIter>
    void copy_from(TIter begin, TIter end)
    {
        copy_to(m_allocation_info.info.pMappedData, begin, end);
    }

    void copy_from(const std::ranges::contiguous_range auto &range)
    {
        copy_to(m_allocation_info.info.pMappedData, range);
    }
};
}