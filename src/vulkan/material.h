#pragma once

#include "glm_config.hpp"
#include "util/no_copy_or_move.h"
#include "vulkan/buffer/uniform_buffer.h"
#include "vulkan/device.h"
#include "vulkan/texture.h"

namespace steeplejack
{
enum class AlphaMode
{
    Opaque,
    Mask,
    Blend
};

class Material : NoCopyOrMove
{
  private:
    // Textures (extend beyond base pass)
    Texture* m_base_color = nullptr;         // sRGB
    Texture* m_normal = nullptr;             // linear
    Texture* m_metallic_roughness = nullptr; // linear (ORM or MR)
    Texture* m_emissive = nullptr;           // sRGB

    // Factors (subset for first step)
    glm::vec4 m_base_color_factor{1.0f, 1.0f, 1.0f, 1.0f};
    float m_alpha_cutoff = 0.5f; // for Mask
    AlphaMode m_alpha_mode = AlphaMode::Opaque;
    bool m_double_sided = false;

    // UBO for material params
    struct UniformBlock
    {
        glm::vec4 baseColorFactor;
        // Room for more later (metallic/roughness/emissive)
    };

    UniformBuffer m_uniform_buffers;
    UniformBlock m_uniform_block{glm::vec4(1.0f)};

  public:
    explicit Material(const Device& device) : m_uniform_buffers(device, sizeof(UniformBlock)) {}

    // Base color
    Texture* base_color() const
    {
        return m_base_color;
    }
    void set_base_color(Texture* tex)
    {
        m_base_color = tex;
    }

    Texture* normal() const
    {
        return m_normal;
    }
    void set_normal(Texture* tex)
    {
        m_normal = tex;
    }

    Texture* metallic_roughness() const
    {
        return m_metallic_roughness;
    }
    void set_metallic_roughness(Texture* tex)
    {
        m_metallic_roughness = tex;
    }

    Texture* emissive() const
    {
        return m_emissive;
    }
    void set_emissive(Texture* tex)
    {
        m_emissive = tex;
    }

    // Factors
    const glm::vec4& base_color_factor() const
    {
        return m_base_color_factor;
    }
    glm::vec4& base_color_factor()
    {
        return m_base_color_factor;
    }

    float alpha_cutoff() const
    {
        return m_alpha_cutoff;
    }
    float& alpha_cutoff()
    {
        return m_alpha_cutoff;
    }

    AlphaMode alpha_mode() const
    {
        return m_alpha_mode;
    }
    void set_alpha_mode(AlphaMode mode)
    {
        m_alpha_mode = mode;
    }

    bool double_sided() const
    {
        return m_double_sided;
    }
    void set_double_sided(bool value)
    {
        m_double_sided = value;
    }

    // UBO interface
    void flush(uint32_t frame_index)
    {
        m_uniform_block.baseColorFactor = m_base_color_factor;
        m_uniform_buffers[frame_index].copy_from(m_uniform_block);
    }

    VkDescriptorBufferInfo* descriptor(uint32_t frame_index)
    {
        return m_uniform_buffers[frame_index].descriptor();
    }
};
} // namespace steeplejack
