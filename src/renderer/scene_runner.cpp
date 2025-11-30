#include "renderer/scene_runner.h"

using namespace steeplejack::renderer;

SceneRunner::SceneRunner(Backend& backend) : m_backend(&backend) {}

Backend& SceneRunner::backend()
{
    return *m_backend;
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
