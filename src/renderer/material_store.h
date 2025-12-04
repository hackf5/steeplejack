#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace steeplejack
{
class Material;
class MaterialFactory;

namespace renderer
{
struct MaterialHandle
{
    std::uint32_t id{kInvalidId};
    static constexpr std::uint32_t kInvalidId = 0;

    [[nodiscard]] bool valid() const
    {
        return id != kInvalidId;
    }
};

struct MaterialView
{
    Material* material{nullptr};
    std::string name;
};

class MaterialStore
{
  public:
    explicit MaterialStore(MaterialFactory& factory);

    MaterialStore(const MaterialStore&) = delete;
    MaterialStore& operator=(const MaterialStore&) = delete;
    MaterialStore(MaterialStore&&) noexcept = delete;
    MaterialStore& operator=(MaterialStore&&) = delete;
    ~MaterialStore() = default;

    MaterialHandle load(const std::string& name, const std::string& gltf_relpath, int material_index = 0);

    [[nodiscard]] std::optional<MaterialView> view(MaterialHandle handle) const;

    void clear();

  private:
    MaterialFactory* m_factory{nullptr};
    std::vector<MaterialView> m_materials;
};
} // namespace renderer
} // namespace steeplejack
