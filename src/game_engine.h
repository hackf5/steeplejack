#pragma once

#include "renderer/backend.h"
#include "renderer/contexts.h"
#include "renderer/scene_runner.h"

#include <chrono>
#include <cstdint>
#include <memory>

namespace steeplejack
{
class GameEngine
{
  private:
    std::unique_ptr<renderer::Backend> m_backend;
    std::unique_ptr<renderer::SceneRunner> m_scene;

    uint32_t m_frame_index = 0;
    std::chrono::system_clock::time_point m_last_time;

    renderer::FrameContext next_frame();
    void rebuild_swapchain();

  public:
    explicit GameEngine(std::unique_ptr<renderer::Backend> backend);

    GameEngine(const GameEngine&) = delete;
    GameEngine& operator=(const GameEngine&) = delete;
    GameEngine(GameEngine&&) = delete;
    GameEngine& operator=(GameEngine&&) = delete;
    ~GameEngine() = default;

    void run();
};
} // namespace steeplejack
