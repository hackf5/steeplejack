#pragma once

#include "node.h"
#include "vulkan/graphics_pipeline.h"
#include "vulkan/shadow_pipeline.h"
#include "util/no_copy_or_move.h"

#include <vulkan/vulkan.h>

namespace steeplejack
{
class Model : NoCopyOrMove
{
  private:
    Node m_root_node;

  public:
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

    void render_shadow(VkCommandBuffer command_buffer, uint32_t frame_index, ShadowPipeline& pipeline, size_t spot_index)
    {
        m_root_node.render_shadow(command_buffer, frame_index, pipeline, spot_index);
    }

    void render(VkCommandBuffer command_buffer, uint32_t frame_index, GraphicsPipeline& pipeline)
    {
        m_root_node.render(command_buffer, frame_index, pipeline);
    }
};
} // namespace steeplejack