#pragma once

#include "glm_config.hpp"
#include "vulkan/buffer/uniform_buffer.h"
#include "vulkan/device.h"
#include "vulkan/texture.h"

namespace steeplejack
{
enum class AlphaMode : std::uint8_t
{
    Opaque,
    Mask,
    Blend
};

class MaterialFactory;

class Material
{
  private:
    Texture* m_base_color = nullptr;         // sRGB
    Texture* m_normal = nullptr;             // linear
    Texture* m_metallic_roughness = nullptr; // linear (ORM or MR)
    Texture* m_emissive = nullptr;           // sRGB

    glm::vec4 m_base_color_factor{1.0F, 1.0F, 1.0F, 1.0F};
    float m_metallic_factor = 1.0F;
    float m_roughness_factor = 1.0F;
    float m_normal_scale = 1.0F;
    float m_alpha_cutoff = 0.5F; // for Mask
    AlphaMode m_alpha_mode = AlphaMode::Opaque;
    bool m_double_sided = false;

    struct UniformBlock
    {
        glm::vec4 baseColorFactor;
        glm::vec4 factors; // x: metallic factor, y: roughness factor, z: normal scale
    };

    UniformBuffer m_uniform_buffers;
    UniformBlock m_uniform_block{.baseColorFactor = glm::vec4(1.0F), .factors = glm::vec4(1.0F)};

  public:
    explicit Material(const Device& device);

    Material(const Material&) = delete;
    Material& operator=(const Material&) = delete;
    Material(Material&&) = delete;
    Material& operator=(Material&&) = delete;
    ~Material() = default;

    [[nodiscard]] Texture* base_color() const;
    [[nodiscard]] Texture* normal() const;
    [[nodiscard]] Texture* metallic_roughness() const;
    [[nodiscard]] Texture* emissive() const;

    [[nodiscard]] const glm::vec4& base_color_factor() const;
    [[nodiscard]] float alpha_cutoff() const;
    [[nodiscard]] AlphaMode alpha_mode() const;
    [[nodiscard]] bool double_sided() const;

    void flush(uint32_t frame_index);

    VkDescriptorBufferInfo* descriptor(uint32_t frame_index);

  private:
    void set_alpha_mode(AlphaMode mode);
    void set_alpha_cutoff(float value);
    void set_base_color_factor(const glm::vec4& value);
    void set_base_color(Texture* tex);
    void set_normal(Texture* tex);
    void set_metallic_roughness(Texture* tex);
    void set_emissive(Texture* tex);
    void set_double_sided(bool value);
    void set_metallic_factor(float value);
    void set_roughness_factor(float value);
    void set_normal_scale(float value);

    friend class MaterialFactory;
};
} // namespace steeplejack
