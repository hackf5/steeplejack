#pragma once

#include <array>

#include <glm/glm.hpp>

#include "util/no_copy_or_move.h"
#include "render_scene.h"
#include "vulkan/device.h"
#include "vulkan/graphics_buffers.h"
#include "vulkan/graphics_pipeline.h"
#include "vulkan/texture_factory.h"
#include "vulkan/vertex.h"

namespace levin
{
class CubesOne: public RenderScene
{
private:
    constexpr static const uint32_t FACES_COUNT = 6;
    constexpr static const uint32_t VERTEXES_PER_FACE = 4;
    constexpr static const uint32_t INDEXES_PER_FACE = 6;
    constexpr static const uint32_t VERTEXES_COUNT = FACES_COUNT * VERTEXES_PER_FACE;
    constexpr static const uint32_t INDEXES_COUNT = FACES_COUNT * INDEXES_PER_FACE;

    typedef std::array<Vertex, VERTEXES_PER_FACE> face_t;
    typedef std::array<Vertex, VERTEXES_COUNT> vertexes_t;
    typedef std::array<Vertex::index_t, INDEXES_COUNT> indexes_t;

    const indexes_t m_indexes;
    const vertexes_t m_vertexes;

    indexes_t create_indexes();
    vertexes_t create_vertexes();

    face_t create_face(uint32_t face);

protected:
    void update(
        uint32_t frame_index,
        float aspect_ratio,
        float time)
        override;

public:
    CubesOne(const Device &device):
        RenderScene(device, "cubes_one.vert", "cubes_one.frag"),
        m_indexes(create_indexes()),
        m_vertexes(create_vertexes())
    {
    }

    virtual void load(
        const Device &device,
        TextureFactory &texture_factory,
        GraphicsBuffers &graphics_buffers)
        override;
};
}