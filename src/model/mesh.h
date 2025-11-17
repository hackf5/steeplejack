#pragma once

#include "glm_config.hpp"
#include "primitive.h"
#include "vulkan/buffer/uniform_buffer.h"
#include "vulkan/device.h"
#include "vulkan/graphics_pipeline.h"
#include "vulkan/material.h"
#include "vulkan/shadow_pipeline.h"

#include <memory>
#include <vector>

namespace steeplejack
{
class Mesh
{
  private:
    struct UniformBlock
    {
        glm::mat4 model;
    };

    UniformBlock m_uniform_block;
    UniformBuffer m_uniform_buffers;
    std::vector<Primitive> m_primitives;
    Material* m_material;

  public:
    Mesh(const Device& device, const std::vector<Primitive>& primitives, Material* material = nullptr) :
        m_uniform_block{},
        m_uniform_buffers(device, sizeof(UniformBlock)),
        m_primitives(primitives),
        m_material(material)
    {
    }

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&&) = delete;
    Mesh& operator=(Mesh&&) = delete;

    const glm::mat4& model() const
    {
        return m_uniform_block.model;
    }
    glm::mat4& model()
    {
        return m_uniform_block.model;
    }

    void set_material(Material* material)
    {
        m_material = material;
    }

    void flush(uint32_t frame_index)
    {
        m_uniform_buffers[frame_index].copy_from(m_uniform_block);
        if (m_material)
        {
            m_material->flush(frame_index);
        }
    }

    void render_shadow(VkCommandBuffer command_buffer, uint32_t frame_index, ShadowPipeline& pipeline)
    {
        pipeline.descriptor_set_layout().write_uniform_buffer(
            m_uniform_buffers[frame_index].descriptor(),
            1); // Model UBO at binding 1

        pipeline.push_descriptor_set(command_buffer);

        for (auto& primitive : m_primitives)
        {
            primitive.render(command_buffer);
        }
    }

    void render(VkCommandBuffer command_buffer, uint32_t frame_index, GraphicsPipeline& pipeline)
    {
        pipeline.descriptor_set_layout().write_uniform_buffer(m_uniform_buffers[frame_index].descriptor(), 1);

        if (m_material)
        {
            // Material params UBO at binding 3
            pipeline.descriptor_set_layout().write_uniform_buffer(m_material->descriptor(frame_index), 3);

            // Base color texture at binding 2 (if present)
            if (m_material->base_color())
            {
                pipeline.descriptor_set_layout().write_combined_image_sampler(
                    m_material->base_color()->descriptor(),
                    2);
            }
            // Normal map at binding 4
            if (m_material->normal())
            {
                pipeline.descriptor_set_layout().write_combined_image_sampler(m_material->normal()->descriptor(), 4);
            }
            // Metallic-Roughness (or ORM) at binding 5
            if (m_material->metallic_roughness())
            {
                pipeline.descriptor_set_layout().write_combined_image_sampler(
                    m_material->metallic_roughness()->descriptor(),
                    5);
            }
            // Emissive at binding 6
            if (m_material->emissive())
            {
                pipeline.descriptor_set_layout().write_combined_image_sampler(m_material->emissive()->descriptor(), 6);
            }
        }

        pipeline.push_descriptor_set(command_buffer);

        for (auto& primitive : m_primitives)
        {
            primitive.render(command_buffer);
        }
    }
};
} // namespace steeplejack
