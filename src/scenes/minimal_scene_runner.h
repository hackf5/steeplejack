#pragma once

#include "renderer/scene_runner.h"

namespace steeplejack
{
// Minimal scene runner for the new ECS rendering path; currently wires up the minimal pipeline config.
class MinimalSceneRunner final : public renderer::SceneRunner
{
  public:
    explicit MinimalSceneRunner(renderer::Backend& backend);

    void load_resources() override;
    void build_scene_backend() override;
    void update(const renderer::FrameContext& frame) override;
    void render(const renderer::RenderContext& render_ctx) override;
};
} // namespace steeplejack
