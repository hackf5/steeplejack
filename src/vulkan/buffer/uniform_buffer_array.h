#pragma once

#include "util/no_copy_or_move.h"
#include "vulkan/buffer/uniform_buffer.h"
#include "vulkan/device.h"

#include <vulkan/vulkan.h>

namespace steeplejack
{
template <typename T> class UniformBufferArray : public UniformBuffer
{
  private:
    const VkDeviceSize m_stride;
    const size_t m_count;

    static VkDeviceSize calculate_stride(const Device& device)
    {
        VkDeviceSize align = device.properties().limits.minUniformBufferOffsetAlignment;
        VkDeviceSize stride = (sizeof(T) + (align - 1)) & ~(align - 1);
        return stride;
    }

  public:
    UniformBufferArray(const Device& device, size_t count) :
        UniformBuffer(device, calculate_stride(device) * count), m_stride(calculate_stride(device)), m_count(count)
    {
    }

    VkDeviceSize stride() const
    {
        return m_stride;
    }

    size_t count() const
    {
        return m_count;
    }

    void copy_from_at(const T& value, size_t item_index, size_t frame_index)
    {
        BufferHost& buffer = (*this)[frame_index];
        buffer.copy_from_at(value, m_stride * static_cast<VkDeviceSize>(item_index));
    };

    VkDescriptorBufferInfo descriptor_at(size_t item_index, size_t frame_index) const
    {
        const BufferHost& buffer = (*this)[frame_index];
        VkDescriptorBufferInfo info = {};
        info.buffer = buffer info.offset = m_stride * static_cast<VkDeviceSize>(item_index);
        info.range = sizeof(T);
        return info;
    }
};
} // namespace steeplejack