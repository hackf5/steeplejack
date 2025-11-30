#pragma once

#include <cstdint>
#include <vulkan/vulkan.h>

namespace steeplejack
{
class ShadowFramebuffers;
class ShadowMapArray;
class ShadowRenderPass;

namespace renderer
{
// Frame simulation context (delta time, frame index, etc.).
struct FrameContext
{
    uint32_t frame_index{0};
    float aspect_ratio{1.0F};
    float delta_seconds{0.0F};
};

struct ShadowContext
{
    ShadowFramebuffers* framebuffers{nullptr};
    ShadowMapArray* shadow_maps{nullptr};
    ShadowRenderPass* render_pass{nullptr};
    VkViewport viewport{};
    VkRect2D scissor{};
};

// Rendering context for a frame; scenes own pass ordering and can use shadow context as needed.
struct RenderContext
{
    VkCommandBuffer command_buffer{VK_NULL_HANDLE};
    VkFramebuffer framebuffer{VK_NULL_HANDLE};
    VkViewport viewport{};
    VkRect2D scissor{};
    uint32_t frame_index{0};
    ShadowContext shadow{};
};
} // namespace renderer
} // namespace steeplejack
