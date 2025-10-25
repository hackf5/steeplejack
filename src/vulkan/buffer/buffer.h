#pragma once

#include <vector>
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include "util/no_copy_or_move.h"
#include "vulkan/device.h"

namespace levin
{
class Buffer: NoCopyOrMove
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

    VkDescriptorBufferInfo m_descriptor;

    AllocationInfo create_allocation_info(VkDeviceSize size);
    VkDescriptorBufferInfo create_descriptor_info() const;

public:
    Buffer(
        const Device &device,
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_AUTO,
        VmaAllocationCreateFlags allocation_flags = 0);
    ~Buffer();

    VkDeviceSize size() const { return m_allocation_info.info.size; }

    operator const VkBuffer() const { return m_allocation_info.buffer; }

    const VkBuffer* ptr() const { return &m_allocation_info.buffer; }

    VkDescriptorBufferInfo *descriptor() { return &m_descriptor; }
};
}