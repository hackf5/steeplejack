#include "buffer.h"

using namespace steeplejack;

Buffer::Buffer(
    const Device& device,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VmaMemoryUsage memory_usage,
    VmaAllocationCreateFlags allocation_flags) :
    m_allocator(device.allocator()),
    m_usage(usage),
    m_memory_usage(memory_usage),
    m_allocation_flags(allocation_flags),
    m_allocation_info(create_allocation_info(size)),
    m_buffer_size(size),
    m_descriptor(create_descriptor_info())
{
}

Buffer::~Buffer()
{
    if (m_allocation_info.buffer != VK_NULL_HANDLE)
    {
        vmaDestroyBuffer(m_allocator, m_allocation_info.buffer, m_allocation_info.allocation);
    }
}

Buffer::Buffer(Buffer&& other) noexcept :
    m_allocator(other.m_allocator),
    m_usage(other.m_usage),
    m_memory_usage(other.m_memory_usage),
    m_allocation_flags(other.m_allocation_flags),
    m_allocation_info(other.m_allocation_info),
    m_buffer_size(other.m_buffer_size),
    m_descriptor(other.m_descriptor)
{
    other.m_allocation_info.buffer = VK_NULL_HANDLE;
    other.m_allocation_info.allocation = nullptr;
    other.m_allocation_info.info = {};
    other.m_descriptor = {};
}

Buffer::AllocationInfo Buffer::create_allocation_info(VkDeviceSize size)
{
    VkBufferCreateInfo buffer_info = {};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = m_usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo alloc_info = {};
    alloc_info.usage = m_memory_usage;
    alloc_info.flags = m_allocation_flags;

    VkBuffer buffer = nullptr;
    VmaAllocation allocation = nullptr;
    VmaAllocationInfo allocation_info;
    if (vmaCreateBuffer(m_allocator, &buffer_info, &alloc_info, &buffer, &allocation, &allocation_info) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create buffer");
    }

    return {.buffer = buffer, .allocation = allocation, .info = allocation_info};
}

VkDescriptorBufferInfo Buffer::create_descriptor_info() const
{
    // Range must be <= buffer size; use requested buffer size rather than allocation size
    return {.buffer = m_allocation_info.buffer, .offset = 0, .range = m_buffer_size};
}
