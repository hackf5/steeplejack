#pragma once

#include <chrono>

#include "util/no_copy_or_move.h"
#include "render_scene.h"
#include "vulkan/device.h"
#include "vulkan/graphics_buffers.h"
#include "vulkan/graphics_pipeline.h"
#include "vulkan/texture_factory.h"

namespace levin
{
class George: public RenderScene
{
protected:
    void update(
        uint32_t frame_index,
        float aspect_ratio,
        float time)
        override;

public:
    George(const Device &device):
        RenderScene(device, "george.vert", "george.frag")
    {
    }

    virtual void load(
        const Device &device,
        TextureFactory &texture_factory,
        GraphicsBuffers &graphics_buffers)
        override;
};
}
