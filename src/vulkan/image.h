#pragma once

#include "device.h"

#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan.h>

namespace steeplejack
{
class Image
{
  public:
    struct ImageInfo
    {
        uint32_t width;
        uint32_t height;
        VkFormat format;
        VkImageUsageFlags usage;
        VkImageTiling tiling;
        VkSampleCountFlagBits samples;
        uint32_t array_layers;
    };

  private:
    struct AllocationInfo
    {
        VkImage image;
        VmaAllocation allocation;
        VmaAllocationInfo info;
    };

    const Device& m_device;
    const ImageInfo m_image_info;
    const AllocationInfo m_allocation_info;

    AllocationInfo create_allocation_info();

  public:
    Image(
        const Device& device,
        uint32_t width,
        uint32_t height,
        VkFormat format,
        VkImageUsageFlags usage,
        VkImageTiling tiling,
        VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT,
        uint32_t array_layers = 1);
    ~Image();

    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;
    Image(Image&&) = delete;
    Image& operator=(Image&&) = delete;

    [[nodiscard]] VkImage vk() const
    {
        return m_allocation_info.image;
    }

    [[nodiscard]] const ImageInfo& image_info() const
    {
        return m_image_info;
    }
};
} // namespace steeplejack
