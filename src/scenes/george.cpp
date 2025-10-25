#include "george.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace steeplejack;

const std::vector<steeplejack::Vertex> vertexes = {
    {
        .pos = {-0.5f, -0.5f, 0.0f},
        .uv = {1.0f, 0.0f},
        .color = {1.0f, 0.0f, 0.0f, 1.0f},
    },
    {
        {0.5f, -0.5f, 0.0f},
        {0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 1.0f},
    },
    {
        {0.5f, 0.5f, 0.0f},
        {0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f, 1.0f},
    },
    {
        {-0.5f, 0.5f, 0.0f},
        {1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f, 1.0f},
    },
};

const std::vector<steeplejack::Vertex::index_t> indexes = {
    0, 1, 2, 2, 3, 0,
    0, 3, 2, 2, 1, 0,
};

void George::load(
    const Device &device,
    TextureFactory &texture_factory,
    GraphicsBuffers &graphics_buffers)
{
    texture_factory.clear();
    texture_factory.load_texture("george", "george.png");

    graphics_buffers.load_vertexes(vertexes);
    graphics_buffers.load_indexes(indexes);

    std::vector<Primitive> primitives = {
        {0, static_cast<uint32_t>(indexes.size())}
    };

    auto &root_node = m_scene.model().root_node();
    auto mesh1 = std::make_unique<Mesh>(
        device,
        primitives,
        texture_factory["george"]);
    auto &child1 = root_node.add_child(std::move(mesh1));
    child1.translation() = glm::vec3(0.0f, 0.0f, 0.0f);

    auto mesh2 = std::make_unique<Mesh>(
        device,
        primitives,
        texture_factory["george"]);
    auto &child2 = root_node.add_child(std::move(mesh2));
    child2.translation() = glm::vec3(0.0f, -1.0f, -1.0f);

    auto &camera = m_scene.camera();
    camera.target() = glm::vec3(0.0f, 0.0f, 0.0f);
    camera.clip_far() = 10.0f;
    camera.clip_near() = 0.1f;
    camera.fov() = 45.0f;
};

void George::update(
    uint32_t frame_index,
    float aspect_ratio,
    float time)
{
    auto rotation = glm::quat_cast(
        glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)));

    // rotate camera around 0,0: only x and y
    auto &camera = m_scene.camera();
    auto camera_rotation = glm::quat_cast(
        glm::rotate(glm::mat4(1.0f), -time * glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
    camera.position() = camera_rotation * glm::vec3(3.0f, 3.0f, 3.0f);
    camera.aspect_ratio() = aspect_ratio;

    auto &node = m_scene.model().root_node();
    node.rotation() = rotation;
    for (auto &child : node.children())
    {
        child->rotation() = rotation;
    }

    m_scene.flush(frame_index);
}