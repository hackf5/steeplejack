#pragma once

#include "buffer_host.h"
#include "vulkan/device.h"

#include <array>
#include <memory>

namespace steeplejack
{

class UniformBuffer
{
  private:
    std::array<std::unique_ptr<BufferHost>, Device::kMaxFramesInFlight> m_buffers;

  public:
    UniformBuffer(const Device& device, VkDeviceSize size);

    UniformBuffer(const UniformBuffer&) = delete;
    UniformBuffer& operator=(const UniformBuffer&) = delete;
    UniformBuffer(UniformBuffer&&) = delete;
    UniformBuffer& operator=(UniformBuffer&&) = delete;
    ~UniformBuffer() = default;

    BufferHost& at(size_t frame_index);

    [[nodiscard]] const BufferHost& at(size_t frame_index) const;
};
} // namespace steeplejack
