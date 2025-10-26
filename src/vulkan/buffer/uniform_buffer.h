#pragma once

#include "buffer_host.h"
#include "util/no_copy_or_move.h"
#include "vulkan/device.h"

#include <array>
#include <functional>
#include <memory>

namespace steeplejack
{
class UniformBuffer : NoCopyOrMove
{
  private:
    typedef std::array<std::unique_ptr<BufferHost>, Device::max_frames_in_flight> buffers_t;

    buffers_t m_buffers;

    buffers_t create_buffers(const Device& device, VkDeviceSize size)
    {
        buffers_t buffers;
        for (auto& buffer : buffers)
        {
            buffer = std::make_unique<BufferHost>(device, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        }

        return buffers;
    }

  public:
    UniformBuffer(const Device& device, VkDeviceSize size) : m_buffers(create_buffers(device, size)) {}

    BufferHost& operator[](size_t index)
    {
        return *m_buffers[index];
    }

    const BufferHost& operator[](size_t index) const
    {
        return *m_buffers[index];
    }
};
} // namespace steeplejack