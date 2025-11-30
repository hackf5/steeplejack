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

// Rendering context for a frame; scenes own pass ordering and can use shadow context as needed.
struct RenderContext
{
    VkCommandBuffer command_buffer{nullptr};
    VkFramebuffer framebuffer{nullptr};
    uint32_t frame_index{0};
};
} // namespace renderer
} // namespace steeplejack
