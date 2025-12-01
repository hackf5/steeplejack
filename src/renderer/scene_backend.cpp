#include "renderer/scene_backend.h"

#include "vulkan/pipeline/graphics_pipeline.h"
#include "vulkan/pipeline/shadow_pipeline.h"

using namespace steeplejack;
using namespace steeplejack::renderer;

SceneBackend::SceneBackend(
    std::unique_ptr<pipeline::GraphicsPipeline> graphics_pipeline,
    std::unique_ptr<pipeline::ShadowPipeline> shadow_pipeline) :
    m_graphics_pipeline(std::move(graphics_pipeline)), m_shadow_pipeline(std::move(shadow_pipeline))
{
}

SceneBackend::~SceneBackend() = default;

pipeline::GraphicsPipeline& SceneBackend::graphics_pipeline() const
{
    return *m_graphics_pipeline;
}

bool SceneBackend::has_shadow() const
{
    return m_shadow_pipeline != nullptr;
}

pipeline::ShadowPipeline* SceneBackend::shadow_pipeline() const
{
    return m_shadow_pipeline.get();
}
