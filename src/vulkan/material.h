#pragma once

#include "util/no_copy_or_move.h"
#include "vulkan/texture.h"

#include <glm/glm.hpp>

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
    // Textures (base pass-only for first step)
    Texture* m_base_color = nullptr; // sRGB

    // Factors (subset for first step)
    glm::vec4 m_base_color_factor{1.0f, 1.0f, 1.0f, 1.0f};
    float m_alpha_cutoff = 0.5f; // for Mask
    AlphaMode m_alpha_mode = AlphaMode::Opaque;
    bool m_double_sided = false;

  public:
    Material() = default;

    // Base color
    Texture* base_color() const { return m_base_color; }
    void set_base_color(Texture* tex) { m_base_color = tex; }

    // Factors
    const glm::vec4& base_color_factor() const { return m_base_color_factor; }
    glm::vec4& base_color_factor() { return m_base_color_factor; }

    float alpha_cutoff() const { return m_alpha_cutoff; }
    float& alpha_cutoff() { return m_alpha_cutoff; }

    AlphaMode alpha_mode() const { return m_alpha_mode; }
    void set_alpha_mode(AlphaMode mode) { m_alpha_mode = mode; }

    bool double_sided() const { return m_double_sided; }
    void set_double_sided(bool value) { m_double_sided = value; }
};
} // namespace steeplejack

