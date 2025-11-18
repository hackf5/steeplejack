#pragma once

#include "node.h"
#include "vulkan/graphics_pipeline.h"
#include "vulkan/shadow_pipeline.h"

#include <vulkan/vulkan.h>

namespace steeplejack
{
class Model
{
  private:
    Node m_root_node;

  public:
    Model() = default;
    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;
    Model(Model&&) = delete;
    Model& operator=(Model&&) = delete;

    const Node& root_node() const
    {
        return m_root_node;
    }
    Node& root_node()
    {
        return m_root_node;
    }

    void flush(uint32_t frame_index)
    {
        m_root_node.flush(frame_index);
    }

    void
    render_shadow(VkCommandBuffer command_buffer, uint32_t frame_index, ShadowPipeline& pipeline, size_t spot_index)
    {
        m_root_node.render_shadow(command_buffer, frame_index, pipeline, spot_index);
    }

    void render(VkCommandBuffer command_buffer, uint32_t frame_index, GraphicsPipeline& pipeline)
    {
        m_root_node.render(command_buffer, frame_index, pipeline);
    }
};
} // namespace steeplejack
