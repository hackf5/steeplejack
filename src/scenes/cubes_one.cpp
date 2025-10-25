#include "cubes_one.h"

#include <functional>
#include <numeric>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace steeplejack;

CubesOne::vertexes_t CubesOne::create_vertexes()
{
    vertexes_t result;
    for (uint32_t f = 0; f < FACES_COUNT; f++)
    {
        auto face = create_face(f);
        std::copy(face.begin(), face.end(), result.begin() + (f * VERTEXES_PER_FACE));
    }
    return result;
}

CubesOne::indexes_t CubesOne::create_indexes()
{
    return {
        0, 2, 1, 2, 3, 1, // bottom
        4, 5, 6, 6, 5, 7, // top
        8, 10, 9, 10, 11, 9, // front
        13, 14, 12, 13, 15, 14, // back
        17, 18, 16, 17, 19, 18, // left
        20, 22, 21, 22, 23, 21, // right
    };
}

CubesOne::face_t CubesOne::create_face(uint32_t face)
{
    static const float min = -0.5f;
    static const float max = 0.5f;

    CubesOne::face_t result;
    for (uint32_t vx = 0; vx < VERTEXES_PER_FACE; vx++)
    {
        auto &vertex = result[vx];

        vertex.pos = {
            (vx & 1) ? max : min,
            (vx & 2) ? max : min,
            (face & 1) ? max : min,
        };

        switch (face)
        {
        case 0: // bottom
        case 1: // top
            break;
        case 2: // front
        case 3: // back
            vertex.pos = glm::vec3(vertex.pos.y, vertex.pos.z, vertex.pos.x);
            break;
        case 4: // left
        case 5: // right
            vertex.pos = glm::vec3(vertex.pos.z, vertex.pos.y, vertex.pos.x);
            break;
        }

        vertex.uv = {
            (vx & 1) ? 0.0f : 1.0f,
            (vx & 2) ? 0.0f : 1.0f,
        };
        vertex.color = {
            (vx & 1) ? 0.0f : 1.0f,
            (vx & 2) ? 0.0f : 1.0f,
            (vx & 4) ? 0.0f : 1.0f,
            1.0f,
        };
    }

    return result;
}


void CubesOne::load(
    const Device &device,
    TextureFactory &texture_factory,
    GraphicsBuffers &graphics_buffers)
{
    texture_factory.clear();
    texture_factory.load_texture("george", "george.png");

    graphics_buffers.load_vertexes(m_vertexes);
    graphics_buffers.load_indexes(m_indexes);

    std::vector<Primitive> primitives = {
        {0, static_cast<uint32_t>(m_indexes.size())}
    };

    std::vector<Primitive> empty = {};

    auto &root_node = m_scene.model().root_node();
    auto &child1 = root_node.add_child();
    child1.add_child(std::make_unique<Mesh>(
        device,
        primitives,
        texture_factory["george"]));

    auto &camera = m_scene.camera();
    camera.target() = glm::vec3(0.0f, 0.0f, 0.0f);
    camera.clip_far() = 10.0f;
    camera.clip_near() = 0.1f;
    camera.fov() = 60.0f;
}

void CubesOne::update(
    uint32_t frame_index,
    float aspect_ratio,
    float time)
{
    auto rotation_x = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    auto rotation_y = glm::rotate(glm::mat4(1.0f), time * glm::radians(60.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    auto rotation_z = glm::rotate(glm::mat4(1.0f), time * glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    auto &camera = m_scene.camera();
    camera.position() = glm::vec3(2.0f, 2.0f, 2.0f);
    camera.aspect_ratio() = aspect_ratio;

    auto &node = m_scene.model().root_node();
    node.rotation() = rotation_x;
    for (auto &child : node.children())
    {
        child->rotation() = rotation_y;
        for (auto &grandchild : child->children())
        {
            grandchild->rotation() = rotation_z;
        }
    }

    m_scene.flush(frame_index);
}
