#include "vulkan/depth_buffer.h"

#include "spdlog/spdlog.h"

using namespace steeplejack;

DepthBuffer::DepthBuffer(const Device& device, const Swapchain& swapchain)
    : image_(device,
             swapchain.extent().width,
             swapchain.extent().height,
             VK_FORMAT_D32_SFLOAT_S8_UINT,
             VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
             VK_IMAGE_TILING_OPTIMAL,
             device.msaa_samples()),
      image_view_(device, image_, VK_IMAGE_ASPECT_DEPTH_BIT) {}

