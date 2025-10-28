// NOLINTBEGIN
#include "george.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace steeplejack;

const std::vector<steeplejack::Vertex> kVertexes = {
    {
        .pos = {-0.5F, -0.5F, 0.0F},
        .uv = {1.0F, 0.0F},
        .color = {1.0F, 0.0F, 0.0F, 1.0F},
    },
    {
        {0.5F, -0.5F, 0.0F},
        {0.0F, 0.0F},
        {0.0F, 1.0F, 0.0F, 1.0F},
    },
    {
        {0.5F, 0.5F, 0.0F},
        {0.0F, 1.0F},
        {0.0F, 0.0F, 1.0F, 1.0F},
    },
    {
        {-0.5F, 0.5F, 0.0F},
        {1.0F, 1.0F},
        {1.0F, 1.0F, 1.0F, 1.0F},
    },
};

const std::vector<steeplejack::Vertex::index_t> kIndexes = {
    0,
    1,
    2,
    2,
    3,
    0,
    0,
    3,
    2,
    2,
    1,
    0,
};

void George::load(const Device& device, TextureFactory& texture_factory, GraphicsBuffers& graphics_buffers)
{
    texture_factory.clear();
    texture_factory.load_texture_from_gltf("rock", "rock_wall_15_1k/rock_wall_15_1k.gltf");

    graphics_buffers.load_vertexes(kVertexes);
    graphics_buffers.load_indexes(kIndexes);

    std::vector<Primitive> const primitives = {{0, static_cast<uint32_t>(kIndexes.size())}};

    auto& root_node = m_scene.model().root_node();
    auto mesh1 = std::make_unique<Mesh>(device, primitives, texture_factory["rock"]);
    auto& child1 = root_node.add_child(std::move(mesh1));
    child1.translation() = glm::vec3(0.0F, 0.0F, 0.0F);

    auto mesh2 = std::make_unique<Mesh>(device, primitives, texture_factory["rock"]);
    auto& child2 = root_node.add_child(std::move(mesh2));
    child2.translation() = glm::vec3(0.0F, -1.0F, -1.0F);

    auto& camera = m_scene.camera();
    camera.target() = glm::vec3(0.0F, 0.0F, 0.0F);
    camera.clip_far() = 10.0F;
    camera.clip_near() = 0.1F;
    camera.fov() = 45.0F;
};

void George::update(uint32_t frame_index, float aspect_ratio, float time)
{
    auto rotation =
        glm::quat_cast(glm::rotate(glm::mat4(1.0F), time * glm::radians(90.0F), glm::vec3(0.0F, 0.0F, 1.0F)));

    // rotate camera around 0,0: only x and y
    auto& camera = m_scene.camera();
    auto camera_rotation =
        glm::quat_cast(glm::rotate(glm::mat4(1.0F), -time * glm::radians(30.0F), glm::vec3(0.0F, 1.0F, 0.0F)));
    camera.position() = camera_rotation * glm::vec3(3.0F, 3.0F, 3.0F);
    camera.aspect_ratio() = aspect_ratio;

    auto& node = m_scene.model().root_node();
    node.rotation() = rotation;
    for (auto& child : node.children())
    {
        child->rotation() = rotation;
    }

    m_scene.flush(frame_index);
}
// NOLINTEND
