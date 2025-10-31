#pragma once

#include "device.h"
#include "image.h"
#include "image_view.h"
#include "util/no_copy_or_move.h"

#include <memory>
#include <vector>
#include <vulkan/vulkan.h>

namespace steeplejack
{
class ShadowMapArray : public NoCopyOrMove
{
  private:
    const Device& m_device;
    const Image m_image;
    const ImageView m_array_image_view;
    const std::vector<std::unique_ptr<ImageView>> m_layer_image_views;

  public:
    ShadowMapArray(
        const Device& device, uint32_t layers, uint32_t resolution = 1024, VkFormat format = VK_FORMAT_D32_SFLOAT);
    ~ShadowMapArray() = default;

    uint32_t layers() const
    {
        return m_image.image_info().array_layers;
    }

    uint32_t resolution() const
    {
        return m_image.image_info().width;
    }

    VkImageView array_view() const
    {
        return m_array_image_view;
    }

    VkImageView layer_view(uint32_t layer) const
    {
        return *m_layer_image_views.at(layer);
    }
};
} // namespace steeplejack