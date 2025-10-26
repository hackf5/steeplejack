#pragma once

#include "device.h"

#include <vulkan/vulkan.h>

namespace steeplejack
{
class Sampler
{
  private:
    const Device& m_device;
    const VkSampler m_sampler;

    VkSampler create_sampler();

  public:
    Sampler(const Device& device);
    ~Sampler();

    operator VkSampler() const
    {
        return m_sampler;
    }
};
} // namespace steeplejack