#pragma once

#include <cstdint>

namespace steeplejack::renderer
{
// Frame simulation context (delta time, frame index, etc.). Extend as needed.
struct FrameContext
{
    uint32_t frame_index{0};
    float aspect_ratio{1.0F};
    float delta_seconds{0.0F};
};

// Rendering context for a frame (command buffer, framebuffer, etc.) will be added later.
} // namespace steeplejack::renderer
