#pragma once

#include "renderer/backend.h"
#include "scenes/ecs_scene_runner.h"

#include <cstdint>
#include <memory>

namespace steeplejack
{
struct FrameContext
{
    uint32_t frame_index{0};
    float aspect_ratio{1.0F};
    float delta_seconds{0.0F};
};

// ECS-focused game engine loop that delegates rendering to scene-owned resources.
class GameEngine
{
  private:
    std::unique_ptr<renderer::Backend> m_backend;
    std::unique_ptr<EcsSceneRunner> m_scene;

  public:
    GameEngine(std::unique_ptr<renderer::Backend> backend, std::unique_ptr<EcsSceneRunner> scene);

    GameEngine(const GameEngine&) = delete;
    GameEngine& operator=(const GameEngine&) = delete;
    GameEngine(GameEngine&&) = delete;
    GameEngine& operator=(GameEngine&&) = delete;
    ~GameEngine() = default;

    void run();
};
} // namespace steeplejack
