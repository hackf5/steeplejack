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

    [[nodiscard]] VkSampler create_sampler() const;

  public:
    explicit ShadowSampler(const Device& device);
    ~ShadowSampler();

    ShadowSampler(const ShadowSampler&) = delete;
    ShadowSampler& operator=(const ShadowSampler&) = delete;
    ShadowSampler(ShadowSampler&&) = delete;
    ShadowSampler& operator=(ShadowSampler&&) = delete;

    [[nodiscard]] VkSampler vk() const
    {
        return m_sampler;
    }
};
} // namespace steeplejack