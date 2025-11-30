#include "renderer/scene_backend.h"

#include "vulkan/descriptor_set_layout.h"
#include "vulkan/graphics_pipeline.h"
#include "vulkan/shadow_pipeline.h"

using namespace steeplejack;
using namespace steeplejack::renderer;

SceneBackend::SceneBackend(
    std::unique_ptr<DescriptorSetLayout> descriptor_set_layout,
    std::unique_ptr<GraphicsPipeline> graphics_pipeline,
    std::unique_ptr<DescriptorSetLayout> shadow_descriptor_set_layout,
    std::unique_ptr<ShadowPipeline> shadow_pipeline) :
    m_descriptor_set_layout(std::move(descriptor_set_layout)),
    m_graphics_pipeline(std::move(graphics_pipeline)),
    m_shadow_descriptor_set_layout(std::move(shadow_descriptor_set_layout)),
    m_shadow_pipeline(std::move(shadow_pipeline))
{
}

SceneBackend::~SceneBackend() = default;

DescriptorSetLayout& SceneBackend::descriptor_set_layout() const
{
    return *m_descriptor_set_layout;
}

GraphicsPipeline& SceneBackend::graphics_pipeline() const
{
    return *m_graphics_pipeline;
}

bool SceneBackend::has_shadow() const
{
    return m_shadow_pipeline != nullptr;
}

DescriptorSetLayout* SceneBackend::shadow_descriptor_set_layout() const
{
    return m_shadow_descriptor_set_layout.get();
}

ShadowPipeline* SceneBackend::shadow_pipeline() const
{
    return m_shadow_pipeline.get();
}