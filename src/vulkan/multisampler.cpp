#include "vulkan/multisampler.h"

#include "spdlog/spdlog.h"

using namespace steeplejack;

Multisampler::Multisampler(const Device& device, const Swapchain& swapchain)
    : device_(device),
      image_(device,
             swapchain.extent().width,
             swapchain.extent().height,
             swapchain.image_format(),
             VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
             VK_IMAGE_TILING_OPTIMAL,
             device.msaa_samples()),
      image_view_(device, image_, VK_IMAGE_ASPECT_COLOR_BIT) {}

