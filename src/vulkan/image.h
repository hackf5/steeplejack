#pragma once

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

#include "util/no_copy_or_move.h"
#include "vulkan/device.h"

namespace steeplejack {

class Image : public NoCopyOrMove {
public:
    struct ImageInfo {
        const uint32_t width;
        const uint32_t height;
        const VkFormat format;
        const VkImageUsageFlags usage;
        const VkImageTiling tiling;
        const VkSampleCountFlagBits samples;
    };

private:
    struct AllocationInfo {
        const VkImage image;
        const VmaAllocation allocation;
        const VmaAllocationInfo info;
    };

    const Device& device_;
    const ImageInfo info_;
    const AllocationInfo alloc_;

    AllocationInfo create_allocation_info();

public:
    Image(const Device& device,
          uint32_t width,
          uint32_t height,
          VkFormat format,
          VkImageUsageFlags usage,
          VkImageTiling tiling,
          VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);
    ~Image();

    operator VkImage() const { return alloc_.image; }
    const ImageInfo& image_info() const { return info_; }
};

} // namespace steeplejack

