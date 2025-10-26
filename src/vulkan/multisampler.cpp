#include "multisampler.h"

#include "spdlog/spdlog.h"

using namespace steeplejack;

Multisampler::Multisampler(const Device& device, const Swapchain& swapchain) :
    m_device(device),
    m_image(device,
            swapchain.extent().width,
            swapchain.extent().height,
            swapchain.image_format(),
            VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            VK_IMAGE_TILING_OPTIMAL,
            device.msaa_samples()),
    m_image_view(device, m_image, VK_IMAGE_ASPECT_COLOR_BIT)
{
}