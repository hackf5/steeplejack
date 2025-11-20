#pragma once

#include "adhoc_queues.h"
#include "buffer/buffer_gpu.h"
#include "device.h"
#include "util/memory.h"
#include "vertex.h"

#include <array>
#include <memory>
#include <ranges>
#include <type_traits>
#include <vulkan/vulkan.h>

namespace steeplejack
{
class GraphicsBuffers
{
  private:
    const Device& m_device;
    const AdhocQueues& m_adhoc_queues;

    std::unique_ptr<BufferGPU> m_vertex_buffer;
    std::unique_ptr<BufferGPU> m_index_buffer;

  public:
    GraphicsBuffers(const Device& device, const AdhocQueues& adhoc_queues);

    GraphicsBuffers(const GraphicsBuffers&) = delete;
    GraphicsBuffers& operator=(const GraphicsBuffers&) = delete;
    GraphicsBuffers(GraphicsBuffers&&) = delete;
    GraphicsBuffers& operator=(GraphicsBuffers&&) = delete;
    ~GraphicsBuffers() = default;

    template <typename TIter> void load_vertexes(TIter begin, TIter end)
    {
        static_assert(std::is_same_v<Vertex, std::decay_t<decltype(*begin)>>, "TIter must be an iterator to Vertex");

        m_vertex_buffer = std::make_unique<BufferGPU>(
            m_device,
            m_adhoc_queues,
            total_bytes(begin, end),
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        m_vertex_buffer->copy_from(begin, end);
    }

    void load_vertexes(const std::ranges::contiguous_range auto& vertexes)
    {
        load_vertexes(std::begin(vertexes), std::end(vertexes));
    }

    template <typename TIter> void load_indexes(TIter begin, TIter end)
    {
        static_assert(
            std::is_same_v<Vertex::Index, std::decay_t<decltype(*begin)>>,
            "TIter must be an iterator to Vertex::Index");

        m_index_buffer = std::make_unique<BufferGPU>(
            m_device,
            m_adhoc_queues,
            total_bytes(begin, end),
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
        m_index_buffer->copy_from(begin, end);
    }

    void load_indexes(const std::ranges::contiguous_range auto& indexes)
    {
        load_indexes(std::begin(indexes), std::end(indexes));
    }

    void bind(VkCommandBuffer command_buffer) const;
};
} // namespace steeplejack
