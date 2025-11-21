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

  public:
    explicit ShadowSampler(const Device& device);
    ~ShadowSampler();

    ShadowSampler(const ShadowSampler&) = delete;
    ShadowSampler& operator=(const ShadowSampler&) = delete;
    ShadowSampler(ShadowSampler&&) = delete;
    ShadowSampler& operator=(ShadowSampler&&) = delete;

    [[nodiscard]] VkSampler vk() const;
};
} // namespace steeplejack
