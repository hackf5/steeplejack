// ECS components scaffold for steeplejack
#pragma once

#include "glm_config.hpp"

#include <cstdint>
#include <entt/entt.hpp>
#include <string>
#include <vector>

namespace steeplejack
{
namespace ecs
{
// Clean ECS draw range, independent of legacy model types
struct DrawRange
{
    std::uint32_t firstIndex{0};
    std::uint32_t indexCount{0};
    std::int32_t vertexOffset{0};
};

// Opaque ids for engine-managed assets
using GeometryId = std::uint32_t;
using MaterialId = std::uint32_t;

// Transform (data-only). World/local are maintained by systems.
struct Transform
{
    glm::vec3 t{0.0f};
    glm::quat r{1.0f, 0.0f, 0.0f, 0.0f};
    glm::vec3 s{1.0f};

    glm::mat4 local{1.0f};
    glm::mat4 world{1.0f};

    bool dirty{true};
};

// Simple hierarchy via intrusive sibling list for cache-friendly traversals.
struct Relationship
{
    entt::entity parent{entt::null};
    entt::entity first{entt::null};
    entt::entity next{entt::null};
    entt::entity prev{entt::null};
    int depth{0};
};

// Renderable mesh description. Geometry is assumed to be already uploaded via engine resources.
struct RenderMesh
{
    GeometryId geometry{0};
    // Index ranges for draw calls (references into shared buffers)
    std::vector<DrawRange> draws{};

    // Whether this object casts a shadow
    bool casts_shadow{true};
};

// Material reference (engine-owned). Keep this as a lightweight handle/pointer.
struct MaterialRef
{
    MaterialId material{0};
};

// Camera parameters (plus cached matrices)
struct Camera
{
    glm::vec3 position{0.0f};
    glm::vec3 target{0.0f};
    float fov{45.0f};
    float aspect{1.0f};
    float clip_near{0.1f};
    float clip_far{10.0f};

    glm::mat4 view{1.0f};
    glm::mat4 proj{1.0f};

    bool dirty{true};
};

// Ambient light (single instance is typical)
struct Ambient
{
    glm::vec3 color{1.0f};
    float intensity{0.1f};
};

// Spotlight definition. Optionally also a shadow caster with fixed index.
struct SpotLight
{
    bool enable{false};

    glm::vec3 position{0.0f};
    float intensity{1.0f};

    glm::vec3 direction{0.0f, 0.0f, -1.0f};
    float innerCos{glm::cos(glm::radians(15.0f))};

    glm::vec3 color{1.0f};
    float outerCos{glm::cos(glm::radians(25.0f))};

    float range{6.0f};
    int shadow_index{-1}; // index into shadow map array; -1 = not assigned
    bool casts_shadow{false};

    // Cached spotlight matrix for rendering
    glm::mat4 viewProj{1.0f};
};

// Tags & metadata
struct MainCamera
{
};

struct ShadowReceiver
{
};

struct Name
{
    std::string value{};
};

} // namespace ecs
} // namespace steeplejack
