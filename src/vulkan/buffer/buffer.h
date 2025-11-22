#pragma once

#include <span>
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan.h>

namespace steeplejack
{
class Device;

class Buffer
{
  private:
    struct AllocationInfo
    {
        VkBuffer buffer;
        VmaAllocation allocation;
        VmaAllocationInfo info;
    };

    VmaAllocator m_allocator;

    VkBufferUsageFlags m_usage;
    VmaMemoryUsage m_memory_usage;
    VmaAllocationCreateFlags m_allocation_flags;

    AllocationInfo m_allocation_info;

    VkDeviceSize m_buffer_size; // requested buffer size used for descriptors

    VkDescriptorBufferInfo m_descriptor;

    AllocationInfo create_allocation_info(VkDeviceSize size);
    [[nodiscard]] VkDescriptorBufferInfo create_descriptor_info() const;

  protected:
    [[nodiscard]] std::span<std::byte> mapped_span() const;

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
    Buffer(Buffer&&) noexcept;
    Buffer& operator=(Buffer&&) = delete;

    [[nodiscard]] VkBuffer vk() const;

    [[nodiscard]] const VkBuffer* vk_ptr() const;

    [[nodiscard]] VkDeviceSize size() const;

    VkDescriptorBufferInfo* descriptor();
};
} // namespace steeplejack
