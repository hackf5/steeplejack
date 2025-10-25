#pragma once

#include <cstdint>
#include <vulkan/vulkan.h>

namespace steeplejack
{
class Primitive
{
private:
    const uint32_t m_index_offset;
    const uint32_t m_index_count;

public:
    Primitive(uint32_t index_offset, uint32_t index_count):
        m_index_offset(index_offset),
        m_index_count(index_count)
    {
    }

    uint32_t index_offset() const { return m_index_offset; }
    uint32_t index_count() const { return m_index_count; }

    void render(VkCommandBuffer command_buffer) const
    {
        vkCmdDrawIndexed(command_buffer, m_index_count, 1, m_index_offset, 0, 0);
    }
};
}