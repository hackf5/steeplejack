#include "buffer_host.h"

#include "vulkan/device.h"

using namespace steeplejack;

BufferHost::BufferHost(const Device& device, VkDeviceSize size, VkBufferUsageFlags usage) :
    Buffer(
        device,
        size,
        usage,
        VMA_MEMORY_USAGE_AUTO,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT)
{
}
