#pragma once

#include "vulkan/material.h"
#include "vulkan/texture_factory.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace steeplejack
{
class MaterialFactory
{
  private:
    TextureFactory& m_textures;
    std::unordered_map<std::string, std::unique_ptr<Material>> m_materials;

  public:
    explicit MaterialFactory(TextureFactory& textures);

    // Clears all materials (does not clear textures)
    void clear();

    void remove(const std::string& name);

    Material* operator[](const std::string& name);

    // Minimal pass: load baseColor from glTF material[material_index]
    // - gltf_relpath is relative to assets/textures/
    // - Creates a Material with only baseColor set; later we can fill normals/ORM/emissive
    Material& load_gltf_material(const std::string& name, const std::string& gltf_relpath, int material_index = 0);

    // Convenience to create an unlit material from a single texture file (relative to assets/textures)
    Material& create_unlit(const std::string& name, const std::string& texture_relpath);
};
} // namespace steeplejack
