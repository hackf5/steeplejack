#include "vulkan/image.h"

#include "spdlog/spdlog.h"

using namespace steeplejack;

Image::Image(const Device& device,
             uint32_t width,
             uint32_t height,
             VkFormat format,
             VkImageUsageFlags usage,
             VkImageTiling tiling,
             VkSampleCountFlagBits samples)
    : device_(device), info_({width, height, format, usage, tiling, samples}), alloc_(create_allocation_info()) {}

Image::~Image() {
    spdlog::info("Destroying image");
    vmaDestroyImage(device_.allocator(), alloc_.image, alloc_.allocation);
}

Image::AllocationInfo Image::create_allocation_info() {
    VkImageCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    ci.imageType = VK_IMAGE_TYPE_2D;
    ci.extent.width = info_.width;
    ci.extent.height = info_.height;
    ci.extent.depth = 1;
    ci.mipLevels = 1;
    ci.arrayLayers = 1;
    ci.format = info_.format;
    ci.tiling = info_.tiling;
    ci.usage = info_.usage;
    ci.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    ci.samples = info_.samples;
    ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo ai{};
    ai.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    VkImage image{};
    VmaAllocation allocation{};
    VmaAllocationInfo ainfo{};
    if (vmaCreateImage(device_.allocator(), &ci, &ai, &image, &allocation, &ainfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create image");
    }
    return {image, allocation, ainfo};
}

