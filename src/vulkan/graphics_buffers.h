#pragma once

#include "adhoc_queues.h"
#include "buffer/buffer_gpu.h"
#include "device.h"
#include "vertex.h"

#include <memory>
#include <span>
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

    void load_vertexes(std::span<const Vertex> vertexes);

    void load_indexes(std::span<const Vertex::Index> indexes);

    void bind(VkCommandBuffer command_buffer) const;
};
} // namespace steeplejack
