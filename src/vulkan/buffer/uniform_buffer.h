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

    BufferHost& operator[](size_t index);

    const BufferHost& operator[](size_t index) const;
};
} // namespace steeplejack
