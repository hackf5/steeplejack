#pragma once

#include "util/no_copy_or_move.h"
#include "vulkan/device.h"

#include <vector>
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan.h>

namespace steeplejack
{
class Buffer : NoCopyOrMove
{
  protected:
    struct AllocationInfo
    {
        const VkBuffer buffer;
        const VmaAllocation allocation;
        const VmaAllocationInfo info;
    };

    const VmaAllocator m_allocator;

    const VkBufferUsageFlags m_usage;
    const VmaMemoryUsage m_memory_usage;
    const VmaAllocationCreateFlags m_allocation_flags;

    const AllocationInfo m_allocation_info;

    const VkDeviceSize m_buffer_size; // requested buffer size used for descriptors

    VkDescriptorBufferInfo m_descriptor;

    AllocationInfo create_allocation_info(VkDeviceSize size);
    VkDescriptorBufferInfo create_descriptor_info() const;

  public:
    Buffer(const Device& device,
           VkDeviceSize size,
           VkBufferUsageFlags usage,
           VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_AUTO,
           VmaAllocationCreateFlags allocation_flags = 0);
    ~Buffer();

    VkDeviceSize size() const
    {
        return m_buffer_size;
    }

    operator VkBuffer() const
    {
        return m_allocation_info.buffer;
    }

    const VkBuffer* ptr() const
    {
        return &m_allocation_info.buffer;
    }

    VkDescriptorBufferInfo* descriptor()
    {
        return &m_descriptor;
    }
};
} // namespace steeplejack
