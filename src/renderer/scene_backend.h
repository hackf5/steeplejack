// NOLINTBEGIN

#pragma once

#include <memory>

namespace steeplejack
{
namespace pipeline
{
class GraphicsPipeline;
class ShadowPipeline;
} // namespace pipeline

namespace renderer
{
class SceneBackend
{
  private:
    std::unique_ptr<pipeline::GraphicsPipeline> m_graphics_pipeline;
    std::unique_ptr<pipeline::ShadowPipeline> m_shadow_pipeline;

  public:
    SceneBackend(
        std::unique_ptr<pipeline::GraphicsPipeline> graphics_pipeline,
        std::unique_ptr<pipeline::ShadowPipeline> shadow_pipeline);

    SceneBackend(const SceneBackend&) = delete;
    SceneBackend& operator=(const SceneBackend&) = delete;
    SceneBackend(SceneBackend&&) = delete;
    SceneBackend& operator=(SceneBackend&&) = delete;
    ~SceneBackend();

    [[nodiscard]] pipeline::GraphicsPipeline& graphics_pipeline() const;
    [[nodiscard]] bool has_shadow() const;
    [[nodiscard]] pipeline::ShadowPipeline* shadow_pipeline() const;
};
} // namespace renderer
} // namespace steeplejack

// NOLINTEND
