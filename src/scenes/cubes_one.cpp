// NOLINTBEGIN
#include "cubes_one.h"

#include "glm_config.hpp"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <imgui.h>
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
        0,  2,  1,  2,  3,
        1, // bottom
        4,  5,  6,  6,  5,
        7, // top
        8,  10, 9,  10, 11,
        9, // front
        13, 14, 12, 13, 15,
        14, // back
        17, 18, 16, 17, 19,
        18, // left
        20, 22, 21, 22, 23,
        21, // right
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
        vertex.color = glm::vec4(1.0F);
    }

    // Compute flat face normal after positions are finalized
    const glm::vec3 e1 = result[1].pos - result[0].pos;
    const glm::vec3 e2 = result[3].pos - result[0].pos;
    glm::vec3 n = glm::normalize(glm::cross(e1, e2));
    for (auto& v : result)
    {
        v.normal = n;
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
    // ImGui lighting controls (persist across frames)
    struct SpotUI
    {
        float radius;
        float speed_deg;
        float inner_deg;
        float outer_deg;
        float intensity;
        float range;
        float color[3];
    };
    static bool ui_init = false;
    static float ambient_color[3] = {1.0f, 1.0f, 1.0f};
    static float ambient_intensity = 0.1f;
    static SpotUI spot0{2.5f, 45.0f, 15.0f, 25.0f, 2.0f, 6.0f, {1.0f, 0.95f, 0.9f}};
    static SpotUI spot1{2.5f, 60.0f, 12.0f, 22.0f, 2.0f, 6.0f, {0.9f, 0.95f, 1.0f}};

    if (!ui_init)
    {
        ambient_color[0] = m_scene.ambient_color().x;
        ambient_color[1] = m_scene.ambient_color().y;
        ambient_color[2] = m_scene.ambient_color().z;
        ambient_intensity = m_scene.ambient_intensity();
        ui_init = true;
    }

    static bool animate_model = false;
    static bool animate_lights = true;

    if (ImGui::Begin("Lighting", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Checkbox("Animate Model", &animate_model);
        ImGui::SameLine();
        ImGui::Checkbox("Animate Lights", &animate_lights);
        if (ImGui::CollapsingHeader("Ambient", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::ColorEdit3("Color", ambient_color);
            ImGui::SliderFloat("Intensity", &ambient_intensity, 0.0f, 1.0f, "%.3f");
        }
        if (ImGui::CollapsingHeader("Spot 0", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::ColorEdit3("Color##0", spot0.color);
            ImGui::SliderFloat("Intensity##0", &spot0.intensity, 0.0f, 10.0f, "%.2f");
            ImGui::SliderFloat("Radius##0", &spot0.radius, 0.5f, 8.0f, "%.2f");
            ImGui::SliderFloat("Speed (deg/s)##0", &spot0.speed_deg, -180.0f, 180.0f, "%.1f");
            ImGui::SliderFloat("Inner (deg)##0", &spot0.inner_deg, 1.0f, 60.0f, "%.1f");
            ImGui::SliderFloat("Outer (deg)##0", &spot0.outer_deg, 1.0f, 80.0f, "%.1f");
            ImGui::SliderFloat("Range##0", &spot0.range, 0.5f, 20.0f, "%.2f");
            if (spot0.outer_deg < spot0.inner_deg)
                spot0.outer_deg = spot0.inner_deg;
        }
        if (ImGui::CollapsingHeader("Spot 1", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::ColorEdit3("Color##1", spot1.color);
            ImGui::SliderFloat("Intensity##1", &spot1.intensity, 0.0f, 10.0f, "%.2f");
            ImGui::SliderFloat("Radius##1", &spot1.radius, 0.5f, 8.0f, "%.2f");
            ImGui::SliderFloat("Speed (deg/s)##1", &spot1.speed_deg, -180.0f, 180.0f, "%.1f");
            ImGui::SliderFloat("Inner (deg)##1", &spot1.inner_deg, 1.0f, 60.0f, "%.1f");
            ImGui::SliderFloat("Outer (deg)##1", &spot1.outer_deg, 1.0f, 80.0f, "%.1f");
            ImGui::SliderFloat("Range##1", &spot1.range, 0.5f, 20.0f, "%.2f");
            if (spot1.outer_deg < spot1.inner_deg)
                spot1.outer_deg = spot1.inner_deg;
        }
    }
    ImGui::End();

    glm::mat4 rotation_x(1.0f), rotation_y(1.0f), rotation_z(1.0f);
    if (animate_model)
    {
        rotation_x = glm::rotate(glm::mat4(1.0F), time * glm::radians(90.0F), glm::vec3(1.0F, 0.0F, 0.0F));
        rotation_y = glm::rotate(glm::mat4(1.0F), time * glm::radians(60.0F), glm::vec3(0.0F, 1.0F, 0.0F));
        rotation_z = glm::rotate(glm::mat4(1.0F), time * glm::radians(30.0F), glm::vec3(0.0F, 0.0F, 1.0F));
    }

    auto& camera = m_scene.camera();
    camera.position() = glm::vec3(3.0F, 3.0F, 3.0F);
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

    // Animate two spotlights using UI parameters
    float t0 = animate_lights ? time : 0.0f;
    glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), t0 * glm::radians(spot0.speed_deg), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::vec3 pos0 = glm::vec3(rotX * glm::vec4(0.0f, 0.0f, spot0.radius, 1.0f));
    auto& s0 = m_scene.spot_at(0);
    s0.position = pos0;
    s0.intensity = spot0.intensity;
    s0.direction = glm::normalize(-pos0);
    s0.innerCos = glm::cos(glm::radians(spot0.inner_deg));
    s0.color = glm::vec3(spot0.color[0], spot0.color[1], spot0.color[2]);
    s0.outerCos = glm::cos(glm::radians(spot0.outer_deg));
    s0.range = spot0.range;

    float t1 = animate_lights ? time : 0.0f;
    glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), t1 * glm::radians(spot1.speed_deg), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 pos1 = glm::vec3(rotY * glm::vec4(spot1.radius, 0.0f, 0.0f, 1.0f));
    auto& s1 = m_scene.spot_at(1);
    s1.position = pos1;
    s1.intensity = spot1.intensity;
    s1.direction = glm::normalize(-pos1);
    s1.innerCos = glm::cos(glm::radians(spot1.inner_deg));
    s1.color = glm::vec3(spot1.color[0], spot1.color[1], spot1.color[2]);
    s1.outerCos = glm::cos(glm::radians(spot1.outer_deg));
    s1.range = spot1.range;

    // (Second spotlight disabled for simplicity)

    // Apply ambient values from UI
    m_scene.ambient_color() = glm::vec3(ambient_color[0], ambient_color[1], ambient_color[2]);
    m_scene.ambient_intensity() = ambient_intensity;

    m_scene.flush(frame_index);
}
// NOLINTEND
