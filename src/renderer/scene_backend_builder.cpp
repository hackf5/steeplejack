#include "renderer/scene_backend_builder.h"

#include "renderer/backend.h"
#include "renderer/scene_backend.h"
#include "vulkan/descriptor_set_layout.h"
#include "vulkan/graphics_pipeline.h"
#include "vulkan/shadow_pipeline.h"

#include <stdexcept>

using namespace steeplejack;
using namespace steeplejack::renderer;

SceneBackendBuilder& SceneBackendBuilder::set_graphics(std::string layout, std::string vertex, std::string fragment)
{
    m_graphics = PipelineConfig{
        .layout = std::move(layout),
        .vertex_shader = std::move(vertex),
        .fragment_shader = std::move(fragment),
    };
    return *this;
}

SceneBackendBuilder& SceneBackendBuilder::set_shadow(std::string layout, std::string vertex, std::string fragment)
{
    m_shadow = PipelineConfig{
        .layout = std::move(layout),
        .vertex_shader = std::move(vertex),
        .fragment_shader = std::move(fragment),
    };
    return *this;
}

std::unique_ptr<SceneBackend> SceneBackendBuilder::build()
{
    if (!m_graphics.has_value())
    {
        throw std::runtime_error("SceneBackendBuilder: graphics pipeline not configured");
    }

    auto graphics_layout = std::make_unique<DescriptorSetLayout>(m_backend->device(), m_graphics->layout);
    auto graphics_pipeline = std::make_unique<GraphicsPipeline>(
        m_backend->device(),
        *graphics_layout,
        m_backend->render_pass(),
        m_graphics->vertex_shader,
        m_graphics->fragment_shader);

    std::unique_ptr<DescriptorSetLayout> shadow_layout;
    std::unique_ptr<ShadowPipeline> shadow_pipeline;
    if (m_shadow.has_value())
    {
        shadow_layout = std::make_unique<DescriptorSetLayout>(m_backend->device(), m_shadow->layout);
        shadow_pipeline = std::make_unique<ShadowPipeline>(
            m_backend->device(),
            *shadow_layout,
            m_backend->shadow_render_pass(),
            m_shadow->vertex_shader,
            m_shadow->fragment_shader);
    }

    return std::make_unique<SceneBackend>(
        std::move(graphics_layout),
        std::move(graphics_pipeline),
        std::move(shadow_layout),
        std::move(shadow_pipeline));
}