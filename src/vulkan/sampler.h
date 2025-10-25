#pragma once

#include <vulkan/vulkan.h>

#include "device.h"

namespace levin
{
class Sampler
{
private:
    const Device &m_device;
    const VkSampler m_sampler;

    VkSampler create_sampler();

public:
    Sampler(const Device &device);
    ~Sampler();

    operator VkSampler () const { return m_sampler; }
};
}