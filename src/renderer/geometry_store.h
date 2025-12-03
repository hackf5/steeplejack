#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include <vector>
#include <vulkan/vulkan.h>

namespace steeplejack
{
class Device;
class AdhocQueues;
class BufferGPU;

namespace renderer
{
struct MeshHandle
{
    std::uint32_t id{kInvalidId};

    static constexpr std::uint32_t kInvalidId = 0;

    [[nodiscard]] bool valid() const { return id != kInvalidId; }
};

struct MeshView
{
    VkBuffer vertex_buffer{VK_NULL_HANDLE};
    VkDeviceSize vertex_offset{0};
    std::uint32_t vertex_count{0};
    std::uint32_t vertex_stride{0};

    VkBuffer index_buffer{VK_NULL_HANDLE};
    VkDeviceSize index_offset{0};
    std::uint32_t index_count{0};
    VkIndexType index_type{VK_INDEX_TYPE_MAX_ENUM};
};

// Scene-scoped geometry atlas. Initial implementation is append-only; future versions can add free-lists/streaming.
class GeometryStore
{
  public:
    GeometryStore(
        const Device& device,
        const AdhocQueues& adhoc_queues,
        VkDeviceSize vertex_capacity_bytes,
        VkDeviceSize index_capacity_bytes);
    ~GeometryStore();

    GeometryStore(const GeometryStore&) = delete;
    GeometryStore& operator=(const GeometryStore&) = delete;
    GeometryStore(GeometryStore&&) noexcept;
    GeometryStore& operator=(GeometryStore&&) = delete;

    MeshHandle upload_mesh_bytes(
        std::span<const std::byte> vertices,
        std::uint32_t vertex_stride,
        std::span<const std::byte> indices,
        VkIndexType index_type);

    // Convenience for common 32-bit index meshes.
    MeshHandle upload_mesh_u32(
        std::span<const std::byte> vertices, std::uint32_t vertex_stride, std::span<const std::uint32_t> indices);

    template <typename TVertex>
    MeshHandle upload_mesh_u32(std::span<const TVertex> vertices, std::span<const std::uint32_t> indices)
    {
        const auto vertex_bytes = std::as_bytes(vertices);
        return upload_mesh_u32(vertex_bytes, static_cast<std::uint32_t>(sizeof(TVertex)), indices);
    }

    [[nodiscard]] std::optional<MeshView> view(MeshHandle handle) const;

    [[nodiscard]] VkBuffer vertex_buffer() const;
    [[nodiscard]] VkBuffer index_buffer() const;

    [[nodiscard]] VkDeviceSize vertex_capacity() const;
    [[nodiscard]] VkDeviceSize index_capacity() const;

    [[nodiscard]] VkDeviceSize vertex_used() const;
    [[nodiscard]] VkDeviceSize index_used() const;

  private:
    const Device* m_device{nullptr};
    const AdhocQueues* m_adhoc_queues{nullptr};

    VkDeviceSize m_vertex_capacity{0};
    VkDeviceSize m_index_capacity{0};
    VkDeviceSize m_vertex_cursor{0};
    VkDeviceSize m_index_cursor{0};

    std::unique_ptr<BufferGPU> m_vertex_buffer;
    std::unique_ptr<BufferGPU> m_index_buffer;

    std::vector<MeshView> m_meshes;

    [[nodiscard]] MeshHandle store_mesh(const MeshView& view);
};
} // namespace renderer
} // namespace steeplejack
