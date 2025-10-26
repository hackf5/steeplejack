#include "depth_buffer.h"

#include "spdlog/spdlog.h"

using namespace steeplejack;

DepthBuffer::DepthBuffer(const Device& device, const Swapchain& swapchain) :
    m_image(device,
            swapchain.extent().width,
            swapchain.extent().height,
            VK_FORMAT_D32_SFLOAT_S8_UINT,
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            VK_IMAGE_TILING_OPTIMAL,
            device.msaa_samples()),
    m_image_view(device, m_image, VK_IMAGE_ASPECT_DEPTH_BIT)
{
}