#pragma once

#include "glm_config.hpp"
#include "primitive.h"
#include "vulkan/buffer/uniform_buffer.h"
#include "vulkan/device.h"
#include "vulkan/graphics_pipeline.h"
#include "vulkan/material.h"
#include "vulkan/shadow_pipeline.h"

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

    const DescriptorSetLayout* m_cached_shadow_layout = nullptr;
    DescriptorSetLayout::BindingHandle m_shadow_model_handle{};

    const DescriptorSetLayout* m_cached_graphics_layout = nullptr;
    DescriptorSetLayout::BindingHandle m_model_handle{};
    DescriptorSetLayout::BindingHandle m_material_params_handle{};
    DescriptorSetLayout::BindingHandle m_base_color_handle{};
    DescriptorSetLayout::BindingHandle m_normal_handle{};
    DescriptorSetLayout::BindingHandle m_metallic_roughness_handle{};
    DescriptorSetLayout::BindingHandle m_emissive_handle{};

    void cache_shadow_bindings(const DescriptorSetLayout& layout)
    {
        m_cached_shadow_layout = &layout;
        m_shadow_model_handle = layout.binding_handle("modelMatrix");
    }

    void cache_graphics_bindings(const DescriptorSetLayout& layout)
    {
        m_cached_graphics_layout = &layout;
        m_model_handle = layout.binding_handle("model");
        m_material_params_handle = layout.binding_handle("materialParams");
        m_base_color_handle = layout.binding_handle("baseColor");
        m_normal_handle = layout.binding_handle("normal");
        m_metallic_roughness_handle = layout.binding_handle("metallicRoughness");
        m_emissive_handle = layout.binding_handle("emissive");
    }

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
    ~Mesh() = default;

    [[nodiscard]] const glm::mat4& model() const
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
        if (m_material != nullptr)
        {
            m_material->flush(frame_index);
        }
    }

    void render_shadow(VkCommandBuffer command_buffer, uint32_t frame_index, ShadowPipeline& pipeline)
    {
        auto& layout = pipeline.descriptor_set_layout();
        if (m_cached_shadow_layout != &layout)
        {
            cache_shadow_bindings(layout);
        }

        layout.write_uniform_buffer(m_uniform_buffers[frame_index].descriptor(), m_shadow_model_handle);

        pipeline.push_descriptor_set(command_buffer);

        for (auto& primitive : m_primitives)
        {
            primitive.render(command_buffer);
        }
    }

    void render(VkCommandBuffer command_buffer, uint32_t frame_index, GraphicsPipeline& pipeline)
    {
        auto& layout = pipeline.descriptor_set_layout();
        if (m_cached_graphics_layout != &layout)
        {
            cache_graphics_bindings(layout);
        }

        layout.write_uniform_buffer(m_uniform_buffers[frame_index].descriptor(), m_model_handle);

        if (m_material != nullptr)
        {
            // Material params UBO at binding 3
            layout.write_uniform_buffer(m_material->descriptor(frame_index), m_material_params_handle);

            // Base color texture at binding 2 (if present)
            if (m_material->base_color() != nullptr)
            {
                layout.write_combined_image_sampler(m_material->base_color()->descriptor(), m_base_color_handle);
            }
            // Normal map at binding 4
            if (m_material->normal() != nullptr)
            {
                layout.write_combined_image_sampler(m_material->normal()->descriptor(), m_normal_handle);
            }
            // Metallic-Roughness (or ORM) at binding 5
            if (m_material->metallic_roughness() != nullptr)
            {
                layout.write_combined_image_sampler(
                    m_material->metallic_roughness()->descriptor(),
                    m_metallic_roughness_handle);
            }
            // Emissive at binding 6
            if (m_material->emissive() != nullptr)
            {
                layout.write_combined_image_sampler(m_material->emissive()->descriptor(), m_emissive_handle);
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
