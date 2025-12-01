#include "renderer/scene_runner.h"

#include <utility>

using namespace steeplejack::renderer;

SceneRunner::SceneRunner(Backend& backend) : m_backend(&backend) {}

Backend& SceneRunner::backend()
{
    return *m_backend;
}

void SceneRunner::set_scene_backend(std::unique_ptr<SceneBackend> scene_backend)
{
    m_scene_backend = std::move(scene_backend);
}

const Backend& SceneRunner::backend() const
{
    return *m_backend;
}

SceneBackend& SceneRunner::scene_backend()
{
    return *m_scene_backend;
}

const SceneBackend& SceneRunner::scene_backend() const
{
    return *m_scene_backend;
}
