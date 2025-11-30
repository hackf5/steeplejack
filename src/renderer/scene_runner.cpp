#include "renderer/scene_runner.h"

namespace steeplejack::renderer
{
SceneBackend* SceneRunner::backend()
{
    return m_backend.get();
}
const SceneBackend* SceneRunner::backend() const
{
    return m_backend.get();
}
} // namespace steeplejack::renderer