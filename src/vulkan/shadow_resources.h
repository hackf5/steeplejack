#pragma once

#include "util/no_copy_or_move.h"
#include "vulkan/buffer/uniform_buffer.h"
#include "vulkan/device.h"

#include <vulkan/vulkan.h>

namespace steeplejack
{
class ShadowResources : NoCopyOrMove
{
  private:
    VkDeviceSize m_stride;
    UniformBuffer m_shadow_matrices_buffer;

    static VkDeviceSize calculate_shadow_matrix_stride(const Device& device);

  public:
    ShadowResources(const Device& device, size_t max_spots);
    ~ShadowResources();

    void copy_shadow_matrix(const glm::mat4& matrix, size_t spot_index, size_t frame_index)
    {
        auto& buffer = m_shadow_matrices_buffer[frame_index];
        buffer.copy_from_at(matrix, m_stride * spot_index);
    };
};
} // namespace steeplejack