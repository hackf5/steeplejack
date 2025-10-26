#include "image.h"

#include "spdlog/spdlog.h"

using namespace steeplejack;

Image::Image(const Device& device,
             uint32_t width,
             uint32_t height,
             VkFormat format,
             VkImageUsageFlags usage,
             VkImageTiling tiling,
             VkSampleCountFlagBits samples) :
    m_device(device),
    m_image_info({width, height, format, usage, tiling, samples}),
    m_allocation_info(create_allocation_info())
{
}

Image::~Image()
{
    spdlog::info("Destroying image");

    vmaDestroyImage(m_device.allocator(), m_allocation_info.image, m_allocation_info.allocation);
}

Image::AllocationInfo Image::create_allocation_info()
{
    VkImageCreateInfo image_info = {};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.extent.width = m_image_info.width;
    image_info.extent.height = m_image_info.height;
    image_info.extent.depth = 1;
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.format = m_image_info.format;
    image_info.tiling = m_image_info.tiling;
    image_info.usage = m_image_info.usage;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.samples = m_image_info.samples;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.flags = 0;

    VmaAllocationCreateInfo image_alloc_info = {};
    image_alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    VkImage image;
    VmaAllocation allocation;
    VmaAllocationInfo allocation_info;
    if (vmaCreateImage(m_device.allocator(), &image_info, &image_alloc_info, &image, &allocation, &allocation_info) !=
        VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create image");
    }

    return {
        image,
        allocation,
        allocation_info,
    };
}