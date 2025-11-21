#include "render_pass_scope.h"

#include <cassert>

using namespace steeplejack;

RenderPassScope::RenderPassScope(VkCommandBuffer command_buffer) : m_command_buffer(command_buffer) {}

RenderPassScope::RenderPassScope(RenderPassScope&& other) noexcept : m_command_buffer(other.m_command_buffer)
{
    assert(other.m_command_buffer != nullptr);
    other.m_command_buffer = nullptr;
}

RenderPassScope::~RenderPassScope()
{
    if (m_command_buffer != nullptr)
    {
        vkCmdEndRenderPass(m_command_buffer);
        m_command_buffer = nullptr;
    }
}
