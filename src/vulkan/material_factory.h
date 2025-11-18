#pragma once

#include "vulkan/material.h"
#include "vulkan/texture_factory.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace tinygltf
{
class Model;
struct Material;
struct TextureInfo;
struct NormalTextureInfo;
} // namespace tinygltf

namespace steeplejack
{
class MaterialFactory
{
  private:
    const Device& m_device;
    TextureFactory& m_textures;
    std::unordered_map<std::string, std::unique_ptr<Material>> m_materials;

    template <typename TexInfo, typename Setter>
    void load_texture_if_present(
        const tinygltf::Model& model,
        const std::string& name,
        const std::string& gltf_relpath,
        const TexInfo& tex_info,
        const std::string& suffix,
        TextureColorSpace color_space,
        Setter&& setter) const;
    void apply_defaults(Material& target);

  public:
    MaterialFactory(const Device& device, TextureFactory& textures);
    MaterialFactory(const MaterialFactory&) = delete;
    MaterialFactory& operator=(const MaterialFactory&) = delete;
    MaterialFactory(MaterialFactory&&) = delete;
    MaterialFactory& operator=(MaterialFactory&&) = delete;
    ~MaterialFactory() = default;

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
