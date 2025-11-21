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

  public:
    explicit Sampler(const Device& device);
    ~Sampler();

    Sampler(const Sampler&) = delete;
    Sampler& operator=(const Sampler&) = delete;
    Sampler(Sampler&&) = delete;
    Sampler& operator=(Sampler&&) = delete;

    [[nodiscard]] VkSampler vk() const;
};
} // namespace steeplejack
