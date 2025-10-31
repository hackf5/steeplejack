#include "shadow_map_array.h"

using namespace steeplejack;

ShadowMapArray::ShadowMapArray(const Device& device, uint32_t layers, uint32_t resolution, VkFormat format) :
    m_device(device),
    m_image(
        device,
        resolution,
        resolution,
        format,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_IMAGE_TILING_OPTIMAL,
        VK_SAMPLE_COUNT_1_BIT,
        layers),
    m_array_image_view(device, m_image, VK_IMAGE_ASPECT_DEPTH_BIT),
    m_layer_image_views(
        [&]()
        {
            std::vector<std::unique_ptr<ImageView>> views;
            views.reserve(layers);
            for (uint32_t layer = 0; layer < layers; ++layer)
            {
                views.emplace_back(std::make_unique<ImageView>(device, m_image, VK_IMAGE_ASPECT_DEPTH_BIT, layer));
            }
            return views;
        }())
{
}