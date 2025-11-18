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

    [[nodiscard]] VkSampler create_sampler() const;

  public:
    explicit Sampler(const Device& device);
    ~Sampler();

    Sampler(const Sampler&) = delete;
    Sampler& operator=(const Sampler&) = delete;
    Sampler(Sampler&&) = delete;
    Sampler& operator=(Sampler&&) = delete;

    [[nodiscard]] VkSampler vk() const
    {
        return m_sampler;
    }
};
} // namespace steeplejack