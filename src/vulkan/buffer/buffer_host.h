#pragma once

#include "buffer.h"
#include "util/memory.h"

#include <cassert>
#include <cstddef>
#include <ranges>
#include <vulkan/vulkan.h>

namespace steeplejack
{
class BufferHost : public Buffer
{
  private:
    void copy_from(void* data, VkDeviceSize size)
    {
        memcpy(m_allocation_info.info.pMappedData, data, size);
    }

  public:
    BufferHost(const Device& device, VkDeviceSize size, VkBufferUsageFlags usage) :
        Buffer(
            device,
            size,
            usage,
            VMA_MEMORY_USAGE_AUTO,
            VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT)
    {
    }

    template <typename T> void copy_from(const T& data)
    {
        copy_to(m_allocation_info.info.pMappedData, data);
    }

    template <typename T> void copy_from_at(const T& data, VkDeviceSize offset)
    {
        assert(offset + sizeof(T) <= m_buffer_size);
        auto* base = static_cast<std::byte*>(m_allocation_info.info.pMappedData);
        std::span<std::byte> mapped{base, static_cast<size_t>(m_buffer_size)};
        copy_to(mapped.subspan(static_cast<size_t>(offset)).data(), data);
    }

    template <typename TIter> void copy_from(TIter begin, TIter end)
    {
        copy_to(m_allocation_info.info.pMappedData, begin, end);
    }

    void copy_from(const std::ranges::contiguous_range auto& range)
    {
        copy_to(m_allocation_info.info.pMappedData, range);
    }
};
} // namespace steeplejack
