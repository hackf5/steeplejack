// NOLINTBEGIN
#include "cubes_one.h"

#include "glm_config.hpp"

#include <algorithm>
#include <cstddef>
#include <imgui.h>

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

    // Compute flat face normal via face index (outward, axis-aligned)
    glm::vec3 n(0.0f);
    switch (face)
    {
    case 0: // bottom
        n = glm::vec3(0.0f, 0.0f, -1.0f);
        break;
    case 1: // top
        n = glm::vec3(0.0f, 0.0f, 1.0f);
        break;
    case 2: // front (+Y)
        n = glm::vec3(0.0f, -1.0f, 0.0f);
        break;
    case 3: // back (-Y)
        n = glm::vec3(0.0f, 1.0f, 0.0f);
        break;
    case 4: // left (-X)
        n = glm::vec3(-1.0f, 0.0f, 0.0f);
        break;
    case 5: // right (+X)
        n = glm::vec3(1.0f, 0.0f, 0.0f);
        break;
    default:
        throw std::runtime_error("Invalid face index in CubesOne::create_face");
    }

    for (auto& v : result)
    {
        v.normal = n;
    }

    return result;
}

void CubesOne::load(const Device& device, MaterialFactory& material_factory, GraphicsBuffers& graphics_buffers)
{
    auto& mat = material_factory.load_gltf("rock", "rock_wall_15_1k/rock_wall_15_1k.gltf");

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
    static SpotUI spot0{2.5f, 45.0f, 5.5f, 14.5f, 2.0f, 6.0f, {1.0f, 0.95f, 0.9f}};
    static SpotUI spot1{2.5f, 60.0f, 12.0f, 22.0f, 0.0f, 6.0f, {0.9f, 0.95f, 1.0f}};

    if (!ui_init)
    {
        ambient_color[0] = m_scene.lights().ambient_color().x;
        ambient_color[1] = m_scene.lights().ambient_color().y;
        ambient_color[2] = m_scene.lights().ambient_color().z;
        ambient_intensity = m_scene.lights().ambient_intensity();
        // Initialize camera UI from current camera
        // Defaults established in load()
        // We will expose these via ImGui below
        ui_init = true;
    }

    static bool animate_model = false;
    static bool animate_lights = true;

    static bool show_spot_markers = true;
    static bool shadows_enabled = true;
    static int debug_mode = 0; // 0=none,1=cone,2=NdotL,3=shadowVis,4=normals
    static int debug_spot = 0;
    // Orbit camera controls
    static float cam_rot_x_deg = 35.0f;
    static float cam_rot_y_deg = 45.0f;
    static float cam_rot_z_deg = 0.0f;
    static float cam_distance = 3.0f;
    if (ImGui::Begin("Lighting", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {

        ImGui::Checkbox("Show Spot Markers", &show_spot_markers);
        ImGui::SameLine();
        ImGui::Checkbox("Shadows Enabled", &shadows_enabled);
        ImGui::Checkbox("Animate Model", &animate_model);
        ImGui::SameLine();
        ImGui::Checkbox("Animate Lights", &animate_lights);
        if (ImGui::CollapsingHeader("Camera (Orbit)", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::SliderFloat("Rot X (deg)", &cam_rot_x_deg, 0.0f, 360.0f, "%.1f");
            ImGui::SliderFloat("Rot Y (deg)", &cam_rot_y_deg, 0.0f, 360.0f, "%.1f");
            ImGui::SliderFloat("Rot Z (deg)", &cam_rot_z_deg, 0.0f, 360.0f, "%.1f");
            ImGui::SliderFloat("Distance", &cam_distance, 0.5f, 20.0f, "%.2f");
        }
        const char* dbg_items[] = {"None", "Cone", "NdotL", "ShadowVis", "Normals", "Normals Pass/Fail"};
        ImGui::Combo("Debug View", &debug_mode, dbg_items, IM_ARRAYSIZE(dbg_items));
        int maxSpot = static_cast<int>(m_scene.lights().spots_size());
        ImGui::SliderInt("Debug Spot", &debug_spot, 0, std::max(0, maxSpot - 1));

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

    // Apply debug/shadow settings via ShadowParams.debugParams
    m_scene.lights().set_shadows_enabled(shadows_enabled);
    m_scene.lights().set_debug_mode(debug_mode);
    m_scene.lights().set_debug_spot_index(debug_spot);

    // Draw simple screen-space markers for each enabled spot
    if (show_spot_markers)
    {
        ImDrawList* dl = ImGui::GetForegroundDrawList();
        const ImVec2 screen = ImGui::GetIO().DisplaySize;
        const glm::mat4 VP = m_scene.camera().proj() * m_scene.camera().view();

        // Project cube center (world origin) to screen once
        bool center_visible = false;
        ImVec2 center_pt{};
        {
            glm::vec4 cclip = VP * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            if (cclip.w > 0.0f)
            {
                glm::vec3 cndc = glm::vec3(cclip) / cclip.w;
                if (cndc.x >= -1.0f && cndc.x <= 1.0f && cndc.y >= -1.0f && cndc.y <= 1.0f)
                {
                    center_visible = true;
                    center_pt = ImVec2((cndc.x * 0.5f + 0.5f) * screen.x, (cndc.y * 0.5f + 0.5f) * screen.y);
                }
            }
        }
        for (size_t i = 0; i < m_scene.lights().spots_size(); ++i)
        {
            const auto& sp = m_scene.lights().spot_at(i);
            if (!sp.enable || sp.intensity <= 0.0f)
                continue;
            glm::vec4 clip = VP * glm::vec4(sp.position, 1.0f);
            if (clip.w <= 0.0f)
                continue;
            glm::vec3 ndc = glm::vec3(clip) / clip.w;
            if (ndc.x < -1.0f || ndc.x > 1.0f || ndc.y < -1.0f || ndc.y > 1.0f)
                continue;
            ImVec2 pt{(ndc.x * 0.5f + 0.5f) * screen.x, (ndc.y * 0.5f + 0.5f) * screen.y};
            ImU32 col = ImColor(sp.color.x, sp.color.y, sp.color.z, 1.0f);
            dl->AddCircleFilled(pt, 6.0f, col, 16);
            dl->AddCircle(pt, 8.0f, col, 16, 2.0f);

            // Draw a line from the spot marker to the cube center
            if (center_visible)
            {
                dl->AddLine(pt, center_pt, col, 2.0f);
            }

            // Draw an arrow indicating spotlight direction
            {
                glm::vec3 dir = glm::normalize(sp.direction);
                // Choose a reasonable world-space arrow length
                float arrow_world_len = std::min(std::max(0.25f, 0.5f * sp.range), 2.0f);
                glm::vec3 tip_world = sp.position + dir * arrow_world_len;

                glm::vec4 tclip = VP * glm::vec4(tip_world, 1.0f);
                if (tclip.w > 0.0f)
                {
                    glm::vec3 tndc = glm::vec3(tclip) / tclip.w;
                    if (tndc.x >= -1.0f && tndc.x <= 1.0f && tndc.y >= -1.0f && tndc.y <= 1.0f)
                    {
                        ImVec2 tip{(tndc.x * 0.5f + 0.5f) * screen.x, (tndc.y * 0.5f + 0.5f) * screen.y};
                        // Main arrow shaft
                        dl->AddLine(pt, tip, col, 2.0f);

                        // Arrow head
                        ImVec2 v{tip.x - pt.x, tip.y - pt.y};
                        float vlen = std::sqrt(v.x * v.x + v.y * v.y);
                        if (vlen > 1.0f)
                        {
                            ImVec2 dir2{v.x / vlen, v.y / vlen};
                            float head_len = 10.0f;
                            float head_w = 6.0f;
                            ImVec2 base{tip.x - dir2.x * head_len, tip.y - dir2.y * head_len};
                            ImVec2 perp{-dir2.y, dir2.x};
                            ImVec2 left{base.x + perp.x * (head_w * 0.5f), base.y + perp.y * (head_w * 0.5f)};
                            ImVec2 right{base.x - perp.x * (head_w * 0.5f), base.y - perp.y * (head_w * 0.5f)};
                            dl->AddTriangleFilled(tip, left, right, col);
                        }
                    }
                }
            }
        }
    }

    glm::mat4 rotation_x(1.0f), rotation_y(1.0f), rotation_z(1.0f);
    if (animate_model)
    {
        rotation_x = glm::rotate(glm::mat4(1.0F), time * glm::radians(90.0F), glm::vec3(1.0F, 0.0F, 0.0F));
        rotation_y = glm::rotate(glm::mat4(1.0F), time * glm::radians(60.0F), glm::vec3(0.0F, 1.0F, 0.0F));
        rotation_z = glm::rotate(glm::mat4(1.0F), time * glm::radians(30.0F), glm::vec3(0.0F, 0.0F, 1.0F));
    }

    auto& camera = m_scene.camera();
    // Orbit camera around origin using XYZ rotations and distance
    glm::mat4 Rx = glm::rotate(glm::mat4(1.0f), glm::radians(cam_rot_x_deg), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 Ry = glm::rotate(glm::mat4(1.0f), glm::radians(cam_rot_y_deg), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 Rz = glm::rotate(glm::mat4(1.0f), glm::radians(cam_rot_z_deg), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 R = Rz * Ry * Rx;
    glm::vec3 base(0.0f, 0.0f, glm::max(0.1f, cam_distance));
    glm::vec3 cam_pos = glm::vec3(R * glm::vec4(base, 1.0f));
    camera.position() = cam_pos;
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
    auto& s0 = m_scene.lights().spot_at(0);
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
    auto& s1 = m_scene.lights().spot_at(1);
    s1.position = pos1;
    s1.intensity = spot1.intensity;
    s1.direction = glm::normalize(-pos1);
    s1.innerCos = glm::cos(glm::radians(spot1.inner_deg));
    s1.color = glm::vec3(spot1.color[0], spot1.color[1], spot1.color[2]);
    s1.outerCos = glm::cos(glm::radians(spot1.outer_deg));
    s1.range = spot1.range;

    // Apply ambient values from UI
    m_scene.lights().ambient_color() = glm::vec3(ambient_color[0], ambient_color[1], ambient_color[2]);
    m_scene.lights().ambient_intensity() = ambient_intensity;

    m_scene.lights().update();

    m_scene.flush(frame_index);
}
// NOLINTEND
