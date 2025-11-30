#include "game_engine.h"

using namespace steeplejack;

GameEngine::GameEngine(std::unique_ptr<renderer::Backend> backend, std::unique_ptr<EcsSceneRunner> scene) :
    m_backend(std::move(backend)), m_scene(std::move(scene))
{
}

void GameEngine::run()
{
    // Stub: the real loop will gather inputs, advance the ECS, and delegate rendering to scene-owned resources.
    m_scene->load(m_backend->device(), m_backend->material_factory(), m_backend->graphics_buffers());
}
