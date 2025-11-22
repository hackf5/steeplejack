#include "image.h"

#include "spdlog/spdlog.h"

using namespace steeplejack;

namespace
{
[[nodiscard]] Image::AllocationInfo create_allocation_info(const Device& device, const Image::ImageInfo& info)
{
    spdlog::info("Creating Image");

    VkImageCreateInfo image_info = {};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.extent.width = info.width;
    image_info.extent.height = info.height;
    image_info.extent.depth = 1;
    image_info.mipLevels = 1;
    image_info.arrayLayers = info.array_layers;
    image_info.format = info.format;
    image_info.tiling = info.tiling;
    image_info.usage = info.usage;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.samples = info.samples;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.flags = 0;

    VmaAllocationCreateInfo image_alloc_info = {};
    image_alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    VkImage image = nullptr;
    VmaAllocation allocation = nullptr;
    VmaAllocationInfo allocation_info;
    if (vmaCreateImage(device.allocator(), &image_info, &image_alloc_info, &image, &allocation, &allocation_info) !=
        VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create image");
    }

    return {
        .image = image,
        .allocation = allocation,
        .info = allocation_info,
    };
}
} // namespace

Image::Image(
    const Device& device,
    uint32_t width,
    uint32_t height,
    VkFormat format,
    VkImageUsageFlags usage,
    VkImageTiling tiling,
    VkSampleCountFlagBits samples,
    uint32_t array_layers) :
    m_device(&device),
    m_image_info(
        {.width = width,
         .height = height,
         .format = format,
         .usage = usage,
         .tiling = tiling,
         .samples = samples,
         .array_layers = array_layers}),
    m_allocation_info(create_allocation_info(device, m_image_info))
{
}

Image::~Image()
{
    spdlog::info("Destroying Image");

    if (m_allocation_info.image != nullptr)
    {
        vmaDestroyImage(m_device->allocator(), m_allocation_info.image, m_allocation_info.allocation);
    }
}

Image::Image(Image&& other) noexcept :
    m_device(other.m_device), m_image_info(other.m_image_info), m_allocation_info(other.m_allocation_info)
{
    other.m_allocation_info.image = nullptr;
    other.m_allocation_info.allocation = nullptr;
}

VkImage Image::vk() const
{
    return m_allocation_info.image;
}

const Image::ImageInfo& Image::image_info() const
{
    return m_image_info;
}
