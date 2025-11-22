#include "vulkan/material.h"

using namespace steeplejack;

Material::Material(const Device& device) : m_uniform_buffers(device, sizeof(UniformBlock)) {}

Texture* Material::base_color() const
{
    return m_base_color;
}

void Material::set_base_color(Texture* tex)
{
    m_base_color = tex;
}

Texture* Material::normal() const
{
    return m_normal;
}

void Material::set_normal(Texture* tex)
{
    m_normal = tex;
}

Texture* Material::metallic_roughness() const
{
    return m_metallic_roughness;
}

void Material::set_metallic_roughness(Texture* tex)
{
    m_metallic_roughness = tex;
}

Texture* Material::emissive() const
{
    return m_emissive;
}

void Material::set_emissive(Texture* tex)
{
    m_emissive = tex;
}

const glm::vec4& Material::base_color_factor() const
{
    return m_base_color_factor;
}

void Material::set_base_color_factor(const glm::vec4& value)
{
    m_base_color_factor = value;
}

float Material::alpha_cutoff() const
{
    return m_alpha_cutoff;
}

void Material::set_alpha_cutoff(float value)
{
    m_alpha_cutoff = value;
}

AlphaMode Material::alpha_mode() const
{
    return m_alpha_mode;
}

void Material::set_alpha_mode(AlphaMode mode)
{
    m_alpha_mode = mode;
}

bool Material::double_sided() const
{
    return m_double_sided;
}

void Material::set_double_sided(bool value)
{
    m_double_sided = value;
}

void Material::flush(uint32_t frame_index)
{
    m_uniform_block.baseColorFactor = m_base_color_factor;
    m_uniform_buffers.at(frame_index).copy_from(m_uniform_block);
}

VkDescriptorBufferInfo* Material::descriptor(uint32_t frame_index)
{
    return m_uniform_buffers.at(frame_index).descriptor();
}
