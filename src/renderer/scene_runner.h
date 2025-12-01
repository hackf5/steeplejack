#pragma once

#include "renderer/backend.h"
#include "renderer/contexts.h"
#include "renderer/scene_backend.h"
#include "renderer/scene_backend_builder.h"

#include <memory>

namespace steeplejack::renderer
{
class SceneRunner
{
  private:
    Backend* m_backend;
    std::unique_ptr<SceneBackend> m_scene_backend;

  protected:
    [[nodiscard]] Backend& backend();
    [[nodiscard]] const Backend& backend() const;

    void set_scene_backend(std::unique_ptr<SceneBackend> scene_backend);
    [[nodiscard]] SceneBackend& scene_backend();
    [[nodiscard]] const SceneBackend& scene_backend() const;

  public:
    explicit SceneRunner(Backend& backend);
    virtual ~SceneRunner() = default;

    SceneRunner(const SceneRunner&) = delete;
    SceneRunner& operator=(const SceneRunner&) = delete;
    SceneRunner(SceneRunner&&) = delete;
    SceneRunner& operator=(SceneRunner&&) = delete;

    virtual void load_resources() = 0;

    virtual void build_scene_backend() = 0;

    virtual void update(const FrameContext& frame) = 0;

    virtual void render(const RenderContext& render_ctx) = 0;
};
} // namespace steeplejack::renderer
