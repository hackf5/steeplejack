#pragma once

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

#include "util/no_copy_or_move.h"
#include "device.h"

namespace steeplejack
{
class Image: public NoCopyOrMove
{
public:
    struct ImageInfo
    {
        const uint32_t width;
        const uint32_t height;
        const VkFormat format;
        const VkImageUsageFlags usage;
        const VkImageTiling tiling;
        const VkSampleCountFlagBits samples;
    };

private:
    struct AllocationInfo
    {
        const VkImage image;
        const VmaAllocation allocation;
        const VmaAllocationInfo info;
    };

    const Device &m_device;
    const ImageInfo m_image_info;
    const AllocationInfo m_allocation_info;

    AllocationInfo create_allocation_info();

public:
    Image(
        const Device &device,
        uint32_t width,
        uint32_t height,
        VkFormat format,
        VkImageUsageFlags usage,
        VkImageTiling tiling,
        VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);
    ~Image();

    operator VkImage() const { return m_allocation_info.image; }

    const ImageInfo &image_info() const { return m_image_info; }
};
}