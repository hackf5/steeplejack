#pragma once

#include "node.h"
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

    void render(VkCommandBuffer command_buffer, uint32_t frame_index, GraphicsPipeline& pipeline)
    {
        m_root_node.render(command_buffer, frame_index, pipeline);
    }
};
} // namespace steeplejack