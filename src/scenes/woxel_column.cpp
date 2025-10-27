// NOLINTBEGIN
#include "woxel_column.h"

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace steeplejack;

namespace
{
struct RingVertex
{
    glm::vec3 pos;
    glm::vec2 uv;
    glm::vec4 color;
};

static inline glm::vec3 polar_to_cartesian(float r, float theta, float z)
{
    float c = std::cos(theta);
    float s = std::sin(theta);
    return {r * c, r * s, z};
}
} // namespace

void WoxelColumn::build_column_mesh()
{
    m_vertices.clear();
    m_indices.clear();

    const std::int32_t N = m_grid.num_sectors;
    const std::int32_t L = m_spec.layers;
    const float dy = m_grid.dy; // vertical height per layer
    const float r0 = m_spec.inner_radius;
    const float r1 = r0 + m_grid.dz; // ring thickness

    // Build outer surface vertices: (L+1) rings × N sectors
    // Vertex layout matches steeplejack::Vertex
    m_vertices.reserve(static_cast<size_t>((L + 1) * N));
    for (std::int32_t k = 0; k <= L; ++k)
    {
        const float z = static_cast<float>(k) * dy;
        const float v = static_cast<float>(k) / static_cast<float>(L > 0 ? L : 1);
        for (std::int32_t i = 0; i < N; ++i)
        {
            const float theta = m_grid.theta_of(i);
            const float u = static_cast<float>(i) / static_cast<float>(N);
            Vertex vx{};
            vx.pos = polar_to_cartesian(r1, theta, z);
            vx.uv = {u, v};
            // Color bands by layer and sector for visibility
            vx.color = {u, v, 1.0f - u, 1.0f};
            m_vertices.emplace_back(vx);
        }
    }

    // Indices for outer surface quads per (i,k)
    m_indices.reserve(static_cast<size_t>(L * N * 6));
    for (std::int32_t k = 0; k < L; ++k)
    {
        const std::uint32_t ring0 = static_cast<std::uint32_t>(k * N);
        const std::uint32_t ring1 = static_cast<std::uint32_t>((k + 1) * N);
        for (std::int32_t i = 0; i < N; ++i)
        {
            const std::uint32_t i0 = static_cast<std::uint32_t>(i);
            const std::uint32_t i1 = static_cast<std::uint32_t>((i + 1) % N);
            const std::uint32_t v00 = ring0 + i0;
            const std::uint32_t v01 = ring0 + i1;
            const std::uint32_t v10 = ring1 + i0;
            const std::uint32_t v11 = ring1 + i1;

            // CCW winding for outer surface
            m_indices.push_back(v00);
            m_indices.push_back(v10);
            m_indices.push_back(v01);

            m_indices.push_back(v01);
            m_indices.push_back(v10);
            m_indices.push_back(v11);
        }
    }
}

void WoxelColumn::load(const Device& device, TextureFactory& texture_factory, GraphicsBuffers& graphics_buffers)
{
    texture_factory.clear();
    texture_factory.load_texture("george", "george.png");

    build_column_mesh();

    graphics_buffers.load_vertexes(m_vertices);
    graphics_buffers.load_indexes(m_indices);

    std::vector<Primitive> const primitives = {{0, static_cast<uint32_t>(m_indices.size())}};

    auto& root_node = m_scene.model().root_node();
    auto mesh = std::make_unique<Mesh>(device, primitives, texture_factory["george"]);
    auto& child = root_node.add_child(std::move(mesh));
    child.translation() = glm::vec3(0.0F, 0.0F, 0.0F);

    auto& camera = m_scene.camera();
    camera.target() = glm::vec3(0.0F, 0.0F, 0.0F);
    camera.clip_far() = 100.0F;
    camera.clip_near() = 0.1F;
    camera.fov() = 60.0F;
}

void WoxelColumn::update(uint32_t frame_index, float aspect_ratio, float time)
{
    auto& camera = m_scene.camera();

    // Orbit camera slowly around the column
    const float R = 6.0f;
    const float ang = time * glm::radians(20.0F);
    camera.position() = glm::vec3(R * std::cos(ang), R * std::sin(ang), 3.0F);
    camera.aspect_ratio() = aspect_ratio;

    m_scene.flush(frame_index);
}
// NOLINTEND

