// NOLINTBEGIN

#pragma once

#include <memory>

namespace steeplejack
{
class DescriptorSetLayout;
class GraphicsPipeline;
class ShadowPipeline;

namespace renderer
{
class SceneBackend
{
  private:
    std::unique_ptr<DescriptorSetLayout> m_descriptor_set_layout;
    std::unique_ptr<GraphicsPipeline> m_graphics_pipeline;
    std::unique_ptr<DescriptorSetLayout> m_shadow_descriptor_set_layout;
    std::unique_ptr<ShadowPipeline> m_shadow_pipeline;

  public:
    SceneBackend(
        std::unique_ptr<DescriptorSetLayout> descriptor_set_layout,
        std::unique_ptr<GraphicsPipeline> graphics_pipeline,
        std::unique_ptr<DescriptorSetLayout> shadow_descriptor_set_layout,
        std::unique_ptr<ShadowPipeline> shadow_pipeline);

    SceneBackend(const SceneBackend&) = delete;
    SceneBackend& operator=(const SceneBackend&) = delete;
    SceneBackend(SceneBackend&&) = delete;
    SceneBackend& operator=(SceneBackend&&) = delete;
    ~SceneBackend();

    [[nodiscard]] DescriptorSetLayout& descriptor_set_layout() const;
    [[nodiscard]] GraphicsPipeline& graphics_pipeline() const;
    [[nodiscard]] bool has_shadow() const;
    [[nodiscard]] DescriptorSetLayout* shadow_descriptor_set_layout() const;
    [[nodiscard]] ShadowPipeline* shadow_pipeline() const;
};
} // namespace renderer
} // namespace steeplejack

// NOLINTEND
