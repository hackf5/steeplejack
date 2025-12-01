#include "renderer/scene_backend_builder.h"

#include "renderer/backend.h"
#include "renderer/scene_backend.h"
#include "vulkan/pipeline/graphics_pipeline.h"
#include "vulkan/pipeline/shadow_pipeline.h"

#include <stdexcept>
#include <utility>

using namespace steeplejack;
using namespace steeplejack::renderer;

SceneBackendBuilder& SceneBackendBuilder::set_graphics(std::string pipeline_name)
{
    m_graphics_pipeline_name = std::move(pipeline_name);
    return *this;
}

SceneBackendBuilder& SceneBackendBuilder::set_shadow(std::string pipeline_name)
{
    m_shadow_pipeline_name = std::move(pipeline_name);
    return *this;
}

std::unique_ptr<SceneBackend> SceneBackendBuilder::build()
{
    if (!m_graphics_pipeline_name.has_value())
    {
        throw std::runtime_error("SceneBackendBuilder: graphics pipeline not configured");
    }

    auto graphics_pipeline = std::make_unique<pipeline::GraphicsPipeline>(
        m_backend->device(),
        m_backend->render_pass(),
        *m_graphics_pipeline_name);

    std::unique_ptr<pipeline::ShadowPipeline> shadow_pipeline;
    if (m_shadow_pipeline_name.has_value())
    {
        shadow_pipeline = std::make_unique<pipeline::ShadowPipeline>(
            m_backend->device(),
            m_backend->shadow_render_pass(),
            *m_shadow_pipeline_name);
    }

    return std::make_unique<SceneBackend>(
        std::move(graphics_pipeline),
        std::move(shadow_pipeline));
}
