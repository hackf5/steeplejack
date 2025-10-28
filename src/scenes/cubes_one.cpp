// NOLINTBEGIN
#include "cubes_one.h"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <numeric>

using namespace steeplejack;

CubesOne::vertexes_t CubesOne::create_vertexes()
{
    vertexes_t result;
    for (uint32_t f = 0; f < FACES_COUNT; f++)
    {
        auto face = create_face(f);
        std::ranges::copy(face, result.begin() + (static_cast<size_t>(f * VERTEXES_PER_FACE)));
    }
    return result;
}

CubesOne::indexes_t CubesOne::create_indexes()
{
    return {
        0,  2,  1,  2,  3,  1,  // bottom
        4,  5,  6,  6,  5,  7,  // top
        8,  10, 9,  10, 11, 9,  // front
        13, 14, 12, 13, 15, 14, // back
        17, 18, 16, 17, 19, 18, // left
        20, 22, 21, 22, 23, 21, // right
    };
}

CubesOne::face_t CubesOne::create_face(uint32_t face)
{
    static const float min = -0.5F;
    static const float max = 0.5F;

    CubesOne::face_t result;
    for (uint32_t vx = 0; vx < VERTEXES_PER_FACE; vx++)
    {
        auto& vertex = result[vx];

        vertex.pos = {
            ((vx & 1) != 0U) ? max : min,
            ((vx & 2) != 0U) ? max : min,
            ((face & 1) != 0U) ? max : min,
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
            ((vx & 1) != 0U) ? 0.0F : 1.0F,
            ((vx & 2) != 0U) ? 0.0F : 1.0F,
        };
        vertex.color = {
            ((vx & 1) != 0U) ? 0.0F : 1.0F,
            ((vx & 2) != 0U) ? 0.0F : 1.0F,
            ((vx & 4) != 0U) ? 0.0F : 1.0F,
            1.0F,
        };
    }

    return result;
}

void CubesOne::load(const Device& device, MaterialFactory& material_factory, GraphicsBuffers& graphics_buffers)
{
    auto& mat = material_factory.load_gltf_material("rock", "rock_wall_15_1k/rock_wall_15_1k.gltf");

    graphics_buffers.load_vertexes(m_vertexes);
    graphics_buffers.load_indexes(m_indexes);

    std::vector<Primitive> const primitives = {{0, static_cast<uint32_t>(m_indexes.size())}};

    std::vector<Primitive> const empty = {};

    auto& root_node = m_scene.model().root_node();
    auto& child1 = root_node.add_child();
    child1.add_child(std::make_unique<Mesh>(device, primitives, &mat));

    auto& camera = m_scene.camera();
    camera.target() = glm::vec3(0.0F, 0.0F, 0.0F);
    camera.clip_far() = 10.0F;
    camera.clip_near() = 0.1F;
    camera.fov() = 60.0F;
}

void CubesOne::update(uint32_t frame_index, float aspect_ratio, float time)
{
    auto rotation_x = glm::rotate(glm::mat4(1.0F), time * glm::radians(90.0F), glm::vec3(1.0F, 0.0F, 0.0F));
    auto rotation_y = glm::rotate(glm::mat4(1.0F), time * glm::radians(60.0F), glm::vec3(0.0F, 1.0F, 0.0F));
    auto rotation_z = glm::rotate(glm::mat4(1.0F), time * glm::radians(30.0F), glm::vec3(0.0F, 0.0F, 1.0F));

    auto& camera = m_scene.camera();
    camera.position() = glm::vec3(2.0F, 2.0F, 2.0F);
    camera.aspect_ratio() = aspect_ratio;

    auto& node = m_scene.model().root_node();
    node.rotation() = rotation_x;
    for (auto& child : node.children())
    {
        child->rotation() = rotation_y;
        for (auto& grandchild : child->children())
        {
            grandchild->rotation() = rotation_z;
        }
    }

    m_scene.flush(frame_index);
}
// NOLINTEND
