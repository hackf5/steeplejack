#pragma once

#include "renderer/backend.h"
#include "renderer/scene_runner.h"

#include <memory>

namespace steeplejack
{
class GameEngine
{
  private:
    std::unique_ptr<renderer::Backend> m_backend;
    std::unique_ptr<renderer::SceneRunner> m_scene;

  public:
    explicit GameEngine(std::unique_ptr<renderer::Backend> backend);

    GameEngine(const GameEngine&) = delete;
    GameEngine& operator=(const GameEngine&) = delete;
    GameEngine(GameEngine&&) = delete;
    GameEngine& operator=(GameEngine&&) = delete;
    ~GameEngine() = default;

    void set_scene(std::unique_ptr<renderer::SceneRunner> scene);

    void run();
};
} // namespace steeplejack
