#pragma once

#include "render_scene.h"
#include "util/no_copy_or_move.h"
#include "vulkan/device.h"
#include "vulkan/graphics_buffers.h"
#include "vulkan/graphics_pipeline.h"
#include "vulkan/material_factory.h"

#include <chrono>

namespace steeplejack
{
class George : public RenderScene
{
  protected:
    void update(uint32_t frame_index, float aspect_ratio, float time) override;

  public:
    George(const Device& device) : RenderScene(device, "george.vert", "george.frag") {}

    virtual void
    load(const Device& device, MaterialFactory& material_factory, GraphicsBuffers& graphics_buffers) override;
};
} // namespace steeplejack
