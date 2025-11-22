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
    auto& layout = pipeline.descriptor_set_layout();
    const auto& model_handle = layout.binding_handle("model");
    const auto& material_params_handle = layout.binding_handle("materialParams");
    const auto& base_color_handle = layout.binding_handle("baseColor");
    const auto& normal_handle = layout.binding_handle("normal");
    const auto& metallic_roughness_handle = layout.binding_handle("metallicRoughness");
    const auto& emissive_handle = layout.binding_handle("emissive");

    // Build draw items
    auto items = build_draw_list(reg);
    if (items.empty())
    {
        return;
    }

    for (const auto& it : items)
    {
        // Model UBO at binding 1
        if (auto* mu = scene.model_ubos.get(it.entity))
        {
            layout.write_uniform_buffer(mu->buffer->at(frame_index).descriptor(), model_handle);
        }

        // Material descriptors (UBO at 3; textures at 2/4/5/6)
        if (auto* mat = scene.materials.get(it.material))
        {
            // Params UBO
            layout.write_uniform_buffer(mat->descriptor(frame_index), material_params_handle);
            // Textures if present
            if (mat->base_color() != nullptr)
            {
                layout.write_combined_image_sampler(mat->base_color()->descriptor(), base_color_handle);
            }
            if (mat->normal() != nullptr)
            {
                layout.write_combined_image_sampler(mat->normal()->descriptor(), normal_handle);
            }
            if (mat->metallic_roughness() != nullptr)
            {
                layout.write_combined_image_sampler(mat->metallic_roughness()->descriptor(), metallic_roughness_handle);
            }
            if (mat->emissive() != nullptr)
            {
                layout.write_combined_image_sampler(mat->emissive()->descriptor(), emissive_handle);
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
