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

  public:
    MaterialFactory(const Device& device, TextureFactory& textures);
    MaterialFactory(const MaterialFactory&) = delete;
    MaterialFactory& operator=(const MaterialFactory&) = delete;
    MaterialFactory(MaterialFactory&&) = delete;
    MaterialFactory& operator=(MaterialFactory&&) = delete;
    ~MaterialFactory() = default;

    Material& load_gltf(const std::string& name, const std::string& gltf_relpath, int material_index = 0);
    void remove(const std::string& name);
    void clear();
};
} // namespace steeplejack
