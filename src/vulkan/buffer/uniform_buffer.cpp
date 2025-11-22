#include "uniform_buffer.h"

#include "vulkan/device.h"

using namespace steeplejack;

namespace
{
auto create_buffers(const Device& device, VkDeviceSize size)
{
    std::array<std::unique_ptr<BufferHost>, Device::kMaxFramesInFlight> buffers;
    for (auto& buffer : buffers)
    {
        buffer = std::make_unique<BufferHost>(device, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    }

    return buffers;
}
} // namespace

UniformBuffer::UniformBuffer(const Device& device, VkDeviceSize size) : m_buffers(create_buffers(device, size)) {}

BufferHost& UniformBuffer::at(size_t frame_index)
{
    return *m_buffers.at(frame_index);
}

const BufferHost& UniformBuffer::at(size_t frame_index) const
{
    return *m_buffers.at(frame_index);
}
