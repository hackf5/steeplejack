// ECS resources scaffold for steeplejack
#pragma once

#include <cstddef>

namespace steeplejack
{
class Device;
class GraphicsBuffers;
class MaterialFactory;
class Lights;

namespace ecs
{
// Lightweight handles for engine-owned assets (extend as needed)
struct MeshHandle
{
    std::size_t id{0}; // index into engine-side storage (placeholder)
};

struct MaterialHandle
{
    std::size_t id{0};
};

// Shared engine resources accessible to systems
struct Resources
{
    const Device* device{nullptr};
    GraphicsBuffers* graphics_buffers{nullptr};
    MaterialFactory* material_factory{nullptr};

    // Optional bridge to legacy lighting until fully ECS-driven
    Lights* legacy_lights{nullptr};
};

} // namespace ecs
} // namespace steeplejack

