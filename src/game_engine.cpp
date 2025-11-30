#include "game_engine.h"

using namespace steeplejack;

GameEngine::GameEngine(std::unique_ptr<renderer::Backend> backend) :
    m_backend(std::move(backend))
{
}

void GameEngine::run()
{
}
