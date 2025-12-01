#pragma once

#include <memory>
#include <optional>
#include <string>

namespace steeplejack::renderer
{
class Backend;
class SceneBackend;

// Builder for scene-specific rendering objects (graphics + optional shadow pipelines/layouts).
class SceneBackendBuilder
{
  private:
    Backend* m_backend;

    std::optional<std::string> m_graphics_pipeline_name;
    std::optional<std::string> m_shadow_pipeline_name;

  public:
    explicit SceneBackendBuilder(Backend& backend) : m_backend(&backend) {}

    SceneBackendBuilder& set_graphics(std::string pipeline_name);
    SceneBackendBuilder& set_shadow(std::string pipeline_name);

    std::unique_ptr<SceneBackend> build();
};
} // namespace steeplejack::renderer
