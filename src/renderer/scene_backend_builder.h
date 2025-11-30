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
    struct PipelineConfig
    {
        std::string layout;
        std::string vertex_shader;
        std::string fragment_shader;
    };

    std::optional<PipelineConfig> m_graphics;
    std::optional<PipelineConfig> m_shadow;

  public:
    explicit SceneBackendBuilder(Backend& backend) : m_backend(&backend) {}

    SceneBackendBuilder& set_graphics(std::string layout, std::string vertex, std::string fragment);
    SceneBackendBuilder& set_shadow(std::string layout, std::string vertex, std::string fragment);

    std::unique_ptr<SceneBackend> build();
};
} // namespace steeplejack::renderer
