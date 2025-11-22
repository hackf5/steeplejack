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
  public:
    BufferHost(const Device& device, VkDeviceSize size, VkBufferUsageFlags usage);

    BufferHost(const BufferHost&) = delete;
    BufferHost& operator=(const BufferHost&) = delete;
    BufferHost(BufferHost&&) noexcept = default;
    BufferHost& operator=(BufferHost&&) = delete;
    ~BufferHost() = default;

    template <typename T> void copy_from(const T& data)
    {
        copy_to(mapped_span().data(), data);
    }

    template <typename T> void copy_from_at(const T& data, VkDeviceSize offset)
    {
        assert(offset + sizeof(T) <= size());
        std::span<std::byte> mapped = mapped_span();
        copy_to(mapped.subspan(static_cast<size_t>(offset)).data(), data);
    }

    template <typename TIter> void copy_from(TIter begin, TIter end)
    {
        copy_to(mapped_span().data(), begin, end);
    }

    void copy_from(const std::ranges::contiguous_range auto& range)
    {
        copy_to(mapped_span().data(), range);
    }
};
} // namespace steeplejack
