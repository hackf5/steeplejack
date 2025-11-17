// Forward render system (initial wiring)
#pragma once

#include "ecs/components.h"
#include "ecs/scene.h"
#include "ecs/systems/build_draw_lists.h"
#include "ecs/systems/flush_ubos.h"
#include "vulkan/graphics_pipeline.h"
#include "vulkan/material.h"

#include <entt/entt.hpp>
#include <vulkan/vulkan.h>

namespace steeplejack
{
class GraphicsPipeline; // forward decl
}

namespace steeplejack::ecs
{
// Renders ECS drawables. Assumes caller has:
// - bound pipeline and vertex/index buffers
// - reset descriptor writes and already bound camera/lights/shadow if desired
inline void render_forward(ecs::Scene& scene, VkCommandBuffer cmd, uint32_t frame_index, GraphicsPipeline& pipeline)
{
    auto& reg = scene.registry;

    // Ensure model UBOs are flushed for this frame (model matrix binding = 1)
    flush_model_ubos(reg, *scene.resources.device, scene.model_ubos, frame_index);

    // Build draw items
    auto items = build_draw_list(reg);
    if (items.empty())
        return;

    for (const auto& it : items)
    {
        // Model UBO at binding 1
        if (auto* mu = scene.model_ubos.get(it.entity))
        {
            pipeline.descriptor_set_layout().write_uniform_buffer((*mu->buffer)[frame_index].descriptor(), 1);
        }

        // Material descriptors (UBO at 3; textures at 2/4/5/6)
        if (auto* mat = scene.materials.get(it.material))
        {
            // Params UBO
            pipeline.descriptor_set_layout().write_uniform_buffer(mat->descriptor(frame_index), 3);
            // Textures if present
            if (mat->base_color())
            {
                pipeline.descriptor_set_layout().write_combined_image_sampler(mat->base_color()->descriptor(), 2);
            }
            if (mat->normal())
            {
                pipeline.descriptor_set_layout().write_combined_image_sampler(mat->normal()->descriptor(), 4);
            }
            if (mat->metallic_roughness())
            {
                pipeline.descriptor_set_layout().write_combined_image_sampler(
                    mat->metallic_roughness()->descriptor(),
                    5);
            }
            if (mat->emissive())
            {
                pipeline.descriptor_set_layout().write_combined_image_sampler(mat->emissive()->descriptor(), 6);
            }
        }

        // Push descriptors for this draw
        pipeline.push_descriptor_set(cmd);

        // Draw all ranges for this geometry (buffers already bound by engine)
        for (std::size_t i = 0; i < it.range_count; ++i)
        {
            const auto& r = it.ranges[i];
            vkCmdDrawIndexed(cmd, r.indexCount, 1, r.firstIndex, r.vertexOffset, 0);
        }
    }
}
} // namespace steeplejack::ecs
