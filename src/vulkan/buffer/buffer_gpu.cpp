#include "buffer_gpu.h"

#include "vulkan/adhoc_queues.h"
#include "vulkan/device.h"

using namespace steeplejack;

BufferGPU::BufferGPU(
    const Device& device, const AdhocQueues& adhoc_queues, VkDeviceSize size, VkBufferUsageFlags usage) :
    Buffer(device, size, usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT), m_device(&device), m_adhoc_queues(&adhoc_queues)
{
}

void BufferGPU::copy_from(const BufferHost& buffer) const
{
    m_adhoc_queues->transfer().run(
        [&](VkCommandBuffer command_buffer)
        {
            VkBufferCopy copy_region = {};
            copy_region.size = buffer.size();
            vkCmdCopyBuffer(command_buffer, buffer.vk(), this->vk(), 1, &copy_region);
        });
}
