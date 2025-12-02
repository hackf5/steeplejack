#include "scenes/minimal_scene_runner.h"

#include "renderer/backend.h"
#include "renderer/scene_backend_builder.h"

#include <glm/gtc/matrix_transform.hpp>
#include <utility>
#include <vulkan/vulkan.h>

using namespace steeplejack;

namespace
{
struct MinimalVertex
{
    glm::vec3 pos;
    glm::vec3 pad; // filler to align UV at offset 24
    glm::vec2 uv;
};

struct CameraBlock
{
    glm::mat4 proj;
    glm::mat4 view;
};

struct ModelBlock
{
    glm::mat4 model;
};
} // namespace

MinimalSceneRunner::MinimalSceneRunner(renderer::Backend& backend) : SceneRunner(backend) {}

void MinimalSceneRunner::load_resources() {}

std::unique_ptr<renderer::SceneBackend> MinimalSceneRunner::build_scene_backend(renderer::SceneBackendBuilder& builder)
{
    return builder.set_graphics("minimal").build();
}

void MinimalSceneRunner::update(const renderer::FrameContext& /*frame_ctx*/) {}

void MinimalSceneRunner::render(const renderer::RenderContext& /*render_ctx*/) {}
