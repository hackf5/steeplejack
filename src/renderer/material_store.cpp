#include "renderer/material_store.h"

#include "vulkan/material.h"
#include "vulkan/material_factory.h"

namespace steeplejack::renderer
{
MaterialStore::MaterialStore(MaterialFactory& factory) : m_factory(&factory) {}

MaterialHandle MaterialStore::load(const std::string& name, const std::string& gltf_relpath, int material_index)
{
    auto& material = m_factory->load_gltf(name, gltf_relpath, material_index);

    // If already present, return existing handle
    for (std::size_t i = 0; i < m_materials.size(); ++i)
    {
        if (m_materials[i].material == &material)
        {
            return MaterialHandle{static_cast<std::uint32_t>(i + 1)};
        }
    }

    m_materials.push_back(MaterialView{.material = &material, .name = name});
    return MaterialHandle{static_cast<std::uint32_t>(m_materials.size())};
}

std::optional<MaterialView> MaterialStore::view(MaterialHandle handle) const
{
    if (!handle.valid() || handle.id > m_materials.size())
    {
        return std::nullopt;
    }
    return m_materials[handle.id - 1];
}

void MaterialStore::clear()
{
    m_materials.clear();
    if (m_factory != nullptr)
    {
        m_factory->clear();
    }
}
} // namespace steeplejack::renderer
