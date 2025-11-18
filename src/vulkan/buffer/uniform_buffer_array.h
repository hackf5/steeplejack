#pragma once

#include "vulkan/buffer/uniform_buffer.h"
#include "vulkan/device.h"

#include <unordered_map>
#include <utility>
#include <vulkan/vulkan.h>

namespace steeplejack
{
template <typename T> class UniformBufferArray : public UniformBuffer
{
  private:
    struct PairHash
    {
        size_t operator()(const std::pair<size_t, size_t>& p) const noexcept
        {
            // simple 64-bit hash combine
            return (p.first * 1315423911U) ^ (p.second + 0x9e3779b97f4a7c15ULL + (p.first << 6) + (p.first >> 2));
        }
    };

    const VkDeviceSize m_stride;
    const size_t m_count;
    std::unordered_map<std::pair<size_t, size_t>, VkDescriptorBufferInfo, PairHash> m_descriptors;

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

    [[nodiscard]] VkDeviceSize stride() const
    {
        return m_stride;
    }

    [[nodiscard]] size_t count() const
    {
        return m_count;
    }

    void copy_from_at(const T& value, size_t item_index, size_t frame_index)
    {
        BufferHost& buffer = (*this)[frame_index];
        buffer.copy_from_at(value, m_stride * static_cast<VkDeviceSize>(item_index));
    };

    VkDescriptorBufferInfo* descriptor_ptr_at(size_t item_index, size_t frame_index)
    {
        auto key = std::pair<size_t, size_t>{item_index, frame_index};
        auto& info = m_descriptors[key]; // default-constructs if missing
        const BufferHost& buffer = (*this)[frame_index];
        info.buffer = buffer; // uses BufferHost::operator VkBuffer()
        info.offset = m_stride * static_cast<VkDeviceSize>(item_index);
        info.range = sizeof(T);
        return &info;
    }
};
} // namespace steeplejack
