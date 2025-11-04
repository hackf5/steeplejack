#include "vulkan/shadow_resources.h"

#include "glm_config.hpp"
#include "spdlog/spdlog.h"

using namespace steeplejack;

ShadowResources::ShadowResources(const Device& device, size_t max_spots) :
    m_stride(calculate_shadow_matrix_stride(device)),
    m_shadow_matrices_buffer(device, m_stride * max_spots)
{
    spdlog::info("Creating Shadow Resources");
}

ShadowResources::~ShadowResources()
{
    spdlog::info("Destroying Shadow Resources");
}

size_t ShadowResources::calculate_shadow_matrix_stride(const Device& device)
{
    VkDeviceSize align = device.properties().limits.minUniformBufferOffsetAlignment;
    VkDeviceSize stride = (sizeof(glm::mat4) + (align - 1)) & ~(align - 1);
    return stride;
}