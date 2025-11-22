#include "graphics_buffers.h"

#include <array>

using namespace steeplejack;

constexpr std::array<VkDeviceSize, 1> kVertexOffsets{0};

GraphicsBuffers::GraphicsBuffers(const Device& device, const AdhocQueues& adhoc_queues) :
    m_device(device), m_adhoc_queues(adhoc_queues), m_vertex_buffer(nullptr), m_index_buffer(nullptr)
{
}

void GraphicsBuffers::load_vertexes(std::span<const Vertex> vertexes)
{
    m_vertex_buffer =
        std::make_unique<BufferGPU>(m_device, m_adhoc_queues, total_bytes(vertexes), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    m_vertex_buffer->copy_from(vertexes);
}

void GraphicsBuffers::load_indexes(std::span<const Vertex::Index> indexes)
{
    m_index_buffer =
        std::make_unique<BufferGPU>(m_device, m_adhoc_queues, total_bytes(indexes), VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    m_index_buffer->copy_from(indexes);
}

void GraphicsBuffers::bind(VkCommandBuffer command_buffer) const
{
    vkCmdBindVertexBuffers(command_buffer, 0, 1, m_vertex_buffer->vk_ptr(), kVertexOffsets.data());
    vkCmdBindIndexBuffer(command_buffer, m_index_buffer->vk(), 0, Vertex::kVkIndexType);
}
