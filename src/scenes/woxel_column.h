#pragma once

#include "render_scene.h"
#include "util/no_copy_or_move.h"
#include "vulkan/device.h"
#include "vulkan/graphics_buffers.h"
#include "vulkan/graphics_pipeline.h"
#include "vulkan/texture_factory.h"
#include "vulkan/vertex.h"

#include "woxel/column.h"
#include "woxel/woxel.h"

#include <vector>

namespace steeplejack
{
class WoxelColumn final : public RenderScene
{
  private:
    const woxel::WoxelGrid m_grid;
    const woxel::ColumnSpec m_spec;

    std::vector<Vertex> m_vertices;
    std::vector<Vertex::index_t> m_indices;

    void build_column_mesh();

  protected:
    void update(uint32_t frame_index, float aspect_ratio, float time) override;

  public:
    WoxelColumn(const Device& device, const woxel::WoxelGrid& grid, const woxel::ColumnSpec& spec) :
        RenderScene(device, "george.vert", "george.frag"), m_grid(grid), m_spec(spec)
    {
    }

    virtual void load(const Device& device, TextureFactory& texture_factory, GraphicsBuffers& graphics_buffers) override;
};
} // namespace steeplejack

