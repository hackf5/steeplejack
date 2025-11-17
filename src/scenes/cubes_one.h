#pragma once

#include "glm_config.hpp"
#include "render_scene.h"
#include "vulkan/device.h"
#include "vulkan/graphics_buffers.h"
#include "vulkan/graphics_pipeline.h"
#include "vulkan/material_factory.h"
#include "vulkan/vertex.h"

#include <array>

namespace steeplejack
{
class CubesOne final : public RenderScene
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

    static indexes_t create_indexes();
    static vertexes_t create_vertexes();

    static face_t create_face(uint32_t face);

  protected:
    void update(uint32_t frame_index, float aspect_ratio, float time) override;

  public:
    CubesOne(const Device& device) :
        RenderScene(device, "default.vert", "default.frag"), m_indexes(create_indexes()), m_vertexes(create_vertexes())
    {
    }

    virtual void
    load(const Device& device, MaterialFactory& material_factory, GraphicsBuffers& graphics_buffers) override;
};
} // namespace steeplejack
