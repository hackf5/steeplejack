#pragma once

#include "renderer/scene_runner.h"
#include "ecs/scene.h"
#include "vulkan/buffer/buffer_gpu.h"
#include "vulkan/buffer/uniform_buffer.h"

#include <entt/entt.hpp>
#include <memory>

namespace steeplejack
{
// Minimal scene runner for the new ECS rendering path; currently wires up the minimal pipeline config.
class MinimalSceneRunner final : public renderer::SceneRunner
{
  private:
    ecs::Scene m_scene{};
    std::unique_ptr<BufferGPU> m_vertex_buffer;
    std::unique_ptr<UniformBuffer> m_camera_ubo;
    std::unique_ptr<UniformBuffer> m_model_ubo;

    entt::entity m_camera{entt::null};
    entt::entity m_triangle{entt::null};

    float m_time{0.0F};

  public:
    explicit MinimalSceneRunner(renderer::Backend& backend);

    void load_resources() override;
    void build_scene_backend() override;
    void update(const renderer::FrameContext& frame) override;
    void render(const renderer::RenderContext& render_ctx) override;
};
} // namespace steeplejack
