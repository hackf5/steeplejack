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
    using BuffersT = std::array<std::unique_ptr<BufferHost>, Device::kMaxFramesInFlight>;

    BuffersT m_buffers;

    static BuffersT create_buffers(const Device& device, VkDeviceSize size)
    {
        BuffersT buffers;
        for (auto& buffer : buffers)
        {
            buffer = std::make_unique<BufferHost>(device, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        }

        return buffers;
    }

  public:
    UniformBuffer(const Device& device, VkDeviceSize size) : m_buffers(create_buffers(device, size)) {}

    UniformBuffer(const UniformBuffer&) = delete;
    UniformBuffer& operator=(const UniformBuffer&) = delete;
    UniformBuffer(UniformBuffer&&) = delete;
    UniformBuffer& operator=(UniformBuffer&&) = delete;
    ~UniformBuffer() = default;

    BufferHost& operator[](size_t index)
    {
        return *m_buffers.at(index);
    }

    const BufferHost& operator[](size_t index) const
    {
        return *m_buffers.at(index);
    }
};
} // namespace steeplejack
