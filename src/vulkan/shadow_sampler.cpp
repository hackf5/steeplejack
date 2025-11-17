#include "shadow_sampler.h"

#include "spdlog/spdlog.h"

#include <stdexcept>

using namespace steeplejack;

ShadowSampler::ShadowSampler(const Device& device) : m_device(device), m_sampler(create_sampler()) {}

ShadowSampler::~ShadowSampler()
{
    spdlog::info("Destroying ShadowSampler");
    vkDestroySampler(m_device.vk(), m_sampler, nullptr);
}

VkSampler ShadowSampler::create_sampler() const
{
    spdlog::info("Creating ShadowSampler");

    VkSamplerCreateInfo sampler_info = {};
    sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_info.magFilter = VK_FILTER_LINEAR;
    sampler_info.minFilter = VK_FILTER_LINEAR;
    sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    sampler_info.anisotropyEnable = VK_FALSE;
    sampler_info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    sampler_info.unnormalizedCoordinates = VK_FALSE;
    sampler_info.compareEnable = VK_TRUE;
    sampler_info.compareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    sampler_info.mipLodBias = 0.0F;
    sampler_info.minLod = 0.0F;
    sampler_info.maxLod = 0.0F;

    VkSampler sampler = nullptr;
    if (vkCreateSampler(m_device.vk(), &sampler_info, nullptr, &sampler) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create shadow sampler");
    }

    return sampler;
}
