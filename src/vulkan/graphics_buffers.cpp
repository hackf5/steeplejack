#include "graphics_buffers.h"

#include <array>
#include <type_traits>

using namespace steeplejack;

constexpr std::array<VkDeviceSize, 1> kVertexOffsets{0};

GraphicsBuffers::GraphicsBuffers(const Device& device, const AdhocQueues& adhoc_queues) :
    m_device(device), m_adhoc_queues(adhoc_queues), m_vertex_buffer(nullptr), m_index_buffer(nullptr)
{
}

void GraphicsBuffers::bind(VkCommandBuffer command_buffer) const
{
    vkCmdBindVertexBuffers(command_buffer, 0, 1, m_vertex_buffer->ptr(), kVertexOffsets.data());
    vkCmdBindIndexBuffer(command_buffer, *m_index_buffer, 0, Vertex::vk_index_type);
}