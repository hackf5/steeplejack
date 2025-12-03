#include "renderer/geometry_store.h"

#include "vulkan/adhoc_queues.h"
#include "vulkan/buffer/buffer_gpu.h"
#include "vulkan/buffer/buffer_host.h"
#include "vulkan/device.h"

#include <stdexcept>

namespace steeplejack::renderer
{
namespace
{
[[nodiscard]] VkDeviceSize align_up(VkDeviceSize value, VkDeviceSize alignment)
{
    if (alignment == 0)
    {
        return value;
    }
    const VkDeviceSize remainder = value % alignment;
    if (remainder == 0)
    {
        return value;
    }
    return value + (alignment - remainder);
}

[[nodiscard]] std::uint32_t index_stride_bytes(VkIndexType index_type)
{
    switch (index_type)
    {
    case VK_INDEX_TYPE_UINT16:
        return 2;
    case VK_INDEX_TYPE_UINT32:
        return 4;
    default:
        throw std::runtime_error("Unsupported index type for geometry store");
    }
}
} // namespace

GeometryStore::GeometryStore(
    const Device& device,
    const AdhocQueues& adhoc_queues,
    VkDeviceSize vertex_capacity_bytes,
    VkDeviceSize index_capacity_bytes) :
    m_device(&device),
    m_adhoc_queues(&adhoc_queues),
    m_vertex_capacity(vertex_capacity_bytes),
    m_index_capacity(index_capacity_bytes),
    m_vertex_buffer(
        std::make_unique<BufferGPU>(device, adhoc_queues, vertex_capacity_bytes, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)),
    m_index_buffer(
        std::make_unique<BufferGPU>(device, adhoc_queues, index_capacity_bytes, VK_BUFFER_USAGE_INDEX_BUFFER_BIT))
{
}

GeometryStore::~GeometryStore() = default;

GeometryStore::GeometryStore(GeometryStore&& other) noexcept = default;

MeshHandle GeometryStore::upload_mesh_bytes(
    std::span<const std::byte> vertices,
    std::uint32_t vertex_stride,
    std::span<const std::byte> indices,
    VkIndexType index_type)
{
    if (vertex_stride == 0)
    {
        throw std::runtime_error("Vertex stride must be non-zero");
    }

    const VkDeviceSize vertex_bytes = vertices.size_bytes();
    const VkDeviceSize index_bytes = indices.size_bytes();
    const std::uint32_t index_stride = index_stride_bytes(index_type);

    if (vertex_bytes % vertex_stride != 0)
    {
        throw std::runtime_error("Vertex byte size is not aligned to stride");
    }
    if (index_bytes % index_stride != 0)
    {
        throw std::runtime_error("Index byte size is not aligned to stride");
    }

    const VkDeviceSize vertex_dst_offset = align_up(m_vertex_cursor, static_cast<VkDeviceSize>(vertex_stride));
    const VkDeviceSize index_dst_offset = align_up(m_index_cursor, static_cast<VkDeviceSize>(index_stride));

    if (vertex_dst_offset + vertex_bytes > m_vertex_capacity)
    {
        throw std::runtime_error("Vertex buffer capacity exceeded");
    }
    if (index_dst_offset + index_bytes > m_index_capacity)
    {
        throw std::runtime_error("Index buffer capacity exceeded");
    }

    if (!vertices.empty())
    {
        BufferHost vertex_staging(*m_device, vertex_bytes, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
        vertex_staging.copy_from(vertices);

        m_adhoc_queues->transfer().run(
            [&](auto command_buffer)
            {
                VkBufferCopy copy{};
                copy.srcOffset = 0;
                copy.dstOffset = vertex_dst_offset;
                copy.size = vertex_bytes;
                vkCmdCopyBuffer(command_buffer, vertex_staging.vk(), m_vertex_buffer->vk(), 1, &copy);
            });
    }

    if (!indices.empty())
    {
        BufferHost index_staging(*m_device, index_bytes, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
        index_staging.copy_from(indices);

        m_adhoc_queues->transfer().run(
            [&](auto command_buffer)
            {
                VkBufferCopy copy{};
                copy.srcOffset = 0;
                copy.dstOffset = index_dst_offset;
                copy.size = index_bytes;
                vkCmdCopyBuffer(command_buffer, index_staging.vk(), m_index_buffer->vk(), 1, &copy);
            });
    }

    m_vertex_cursor = vertex_dst_offset + vertex_bytes;
    m_index_cursor = index_dst_offset + index_bytes;

    MeshView view{};
    view.vertex_buffer = m_vertex_buffer->vk();
    view.vertex_offset = vertex_dst_offset;
    view.vertex_count = static_cast<std::uint32_t>(vertex_bytes / vertex_stride);
    view.vertex_stride = vertex_stride;

    view.index_buffer = m_index_buffer->vk();
    view.index_offset = index_dst_offset;
    view.index_type = index_type;
    view.index_count = static_cast<std::uint32_t>(index_bytes / index_stride);

    return store_mesh(view);
}

MeshHandle GeometryStore::upload_mesh_u32(
    std::span<const std::byte> vertices, std::uint32_t vertex_stride, std::span<const std::uint32_t> indices)
{
    const auto index_bytes = std::as_bytes(indices);
    return upload_mesh_bytes(vertices, vertex_stride, index_bytes, VK_INDEX_TYPE_UINT32);
}

std::optional<MeshView> GeometryStore::view(MeshHandle handle) const
{
    if (!handle.valid() || handle.id > m_meshes.size())
    {
        return std::nullopt;
    }
    return m_meshes[handle.id - 1];
}

VkBuffer GeometryStore::vertex_buffer() const
{
    return m_vertex_buffer ? m_vertex_buffer->vk() : VK_NULL_HANDLE;
}

VkBuffer GeometryStore::index_buffer() const
{
    return m_index_buffer ? m_index_buffer->vk() : VK_NULL_HANDLE;
}

VkDeviceSize GeometryStore::vertex_capacity() const
{
    return m_vertex_capacity;
}

VkDeviceSize GeometryStore::index_capacity() const
{
    return m_index_capacity;
}

VkDeviceSize GeometryStore::vertex_used() const
{
    return m_vertex_cursor;
}

VkDeviceSize GeometryStore::index_used() const
{
    return m_index_cursor;
}

MeshHandle GeometryStore::store_mesh(const MeshView& view)
{
    m_meshes.push_back(view);
    return MeshHandle{static_cast<std::uint32_t>(m_meshes.size())};
}
} // namespace steeplejack::renderer
