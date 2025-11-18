#pragma once

#include "vulkan/device.h"

#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan.h>

namespace steeplejack
{
class Buffer
{
  protected:
    struct AllocationInfo
    {
        VkBuffer buffer;
        VmaAllocation allocation;
        VmaAllocationInfo info;
    };

    const VmaAllocator m_allocator;

    const VkBufferUsageFlags m_usage;
    const VmaMemoryUsage m_memory_usage;
    const VmaAllocationCreateFlags m_allocation_flags;

    AllocationInfo m_allocation_info;

    const VkDeviceSize m_buffer_size; // requested buffer size used for descriptors

    VkDescriptorBufferInfo m_descriptor;

    AllocationInfo create_allocation_info(VkDeviceSize size);
    [[nodiscard]] VkDescriptorBufferInfo create_descriptor_info() const;

  public:
    Buffer(
        const Device& device,
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_AUTO,
        VmaAllocationCreateFlags allocation_flags = 0);
    ~Buffer();

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    Buffer(Buffer&&) = delete;
    Buffer& operator=(Buffer&&) = delete;

    [[nodiscard]] VkDeviceSize size() const
    {
        return m_buffer_size;
    }

    [[nodiscard]] VkBuffer vk() const
    {
        return m_allocation_info.buffer;
    }

    [[nodiscard]] const VkBuffer* vk_ptr() const
    {
        return &m_allocation_info.buffer;
    }

    VkDescriptorBufferInfo* descriptor()
    {
        return &m_descriptor;
    }
};
} // namespace steeplejack
