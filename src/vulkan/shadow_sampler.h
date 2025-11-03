#pragma once

#include "device.h"

#include <vulkan/vulkan.h>

namespace steeplejack
{
class ShadowSampler
{
  private:
    const Device& m_device;
    const VkSampler m_sampler;

    VkSampler create_sampler();

  public:
    ShadowSampler(const Device& device);
    ~ShadowSampler();

    operator VkSampler() const
    {
        return m_sampler;
    }
};
} // namespace steeplejack