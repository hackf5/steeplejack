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
    std::unique_ptr<SceneBackend> m_backend;

  protected:
    [[nodiscard]] SceneBackend* backend();
    [[nodiscard]] const SceneBackend* backend() const;

  public:
    SceneRunner() = default;
    virtual ~SceneRunner() = default;

    SceneRunner(const SceneRunner&) = delete;
    SceneRunner& operator=(const SceneRunner&) = delete;
    SceneRunner(SceneRunner&&) = delete;
    SceneRunner& operator=(SceneRunner&&) = delete;

    virtual void load_resources(const Backend& backend) = 0;

    virtual void build_backend(const Backend& backend) = 0;

    virtual void update(const FrameContext& frame) = 0;

    virtual void render(const RenderContext& render_ctx) = 0;
};
} // namespace steeplejack::renderer