#include "scenes/minimal_scene_runner.h"

#include "renderer/backend.h"
#include "renderer/scene_backend_builder.h"

#include <utility>

using namespace steeplejack;

MinimalSceneRunner::MinimalSceneRunner(renderer::Backend& backend) : SceneRunner(backend) {}

void MinimalSceneRunner::load_resources()
{
    // TODO: upload minimal geometry/materials once the scene is defined
}

void MinimalSceneRunner::build_scene_backend()
{
    renderer::SceneBackendBuilder builder(backend());
    auto scene_backend = builder.set_graphics("minimal").build();
    set_scene_backend(std::move(scene_backend));
}

void MinimalSceneRunner::update(const renderer::FrameContext& /*frame*/)
{
    // TODO: update scene state
}

void MinimalSceneRunner::render(const renderer::RenderContext& /*render_ctx*/)
{
    // TODO: record draw commands using the minimal pipeline
}
