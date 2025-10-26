#pragma once

#include <cstdint>
#include <vector>
#include <stdexcept>
#include <functional>

#include "woxel/woxel.h"

namespace steeplejack
{
// Chunk dimensions over (theta, z, radial)
struct ChunkDims
{
    std::int32_t Ctheta = 32; // sectors per chunk
    std::int32_t Cz = 32;     // vertical layers per chunk
    std::int32_t Cr = 1;      // radial layers per chunk (PoC)

    std::int64_t cell_count() const
    {
        return static_cast<std::int64_t>(Ctheta) * Cz * Cr;
    }
};

// Address of a chunk in chunk-space (theta, z, radial)
struct ChunkAddress
{
    std::int32_t ctheta = 0;
    std::int32_t cz = 0;
    std::int32_t cr = 0;

    bool operator==(const ChunkAddress &o) const
    { return ctheta == o.ctheta && cz == o.cz && cr == o.cr; }
};

struct ChunkAddressHasher
{
    std::size_t operator()(const ChunkAddress &a) const noexcept
    {
        std::size_t h = 1469598103934665603ull;
        auto mix = [&](std::uint64_t v){ h ^= v + 0x9e3779b97f4a7c15ull + (h<<6) + (h>>2); };
        mix(static_cast<std::uint64_t>(static_cast<std::int64_t>(a.ctheta)));
        mix(static_cast<std::uint64_t>(static_cast<std::int64_t>(a.cz)));
        mix(static_cast<std::uint64_t>(static_cast<std::int64_t>(a.cr)));
        return h;
    }
};

// Simple cell payload for PoC: 0 = empty, >0 = material id
using WoxelCell = std::uint8_t;

class WoxelChunk
{
private:
    WoxelSpaceSpec m_spec;
    ChunkDims m_dims;
    ChunkAddress m_addr;
    std::vector<WoxelCell> m_cells; // dense storage

    std::int64_t linear_index(std::int32_t i_theta, std::int32_t i_z, std::int32_t i_r) const
    {
        // Expect local indices in [0,C*)
        if (i_theta < 0 || i_theta >= m_dims.Ctheta ||
            i_z < 0 || i_z >= m_dims.Cz ||
            i_r < 0 || i_r >= m_dims.Cr)
        {
            throw std::out_of_range("WoxelChunk: local index out of range");
        }

        // Layout: ((i_r * Cz) + i_z) * Ctheta + i_theta
        return static_cast<std::int64_t>((i_r * m_dims.Cz + i_z) * m_dims.Ctheta + i_theta);
    }

public:
    WoxelChunk(const WoxelSpaceSpec &spec, const ChunkDims &dims, const ChunkAddress &addr)
        : m_spec(spec)
        , m_dims(dims)
        , m_addr(addr)
        , m_cells(static_cast<std::size_t>(dims.cell_count()), 0)
    {
    }

    const ChunkDims &dims() const { return m_dims; }
    const ChunkAddress &address() const { return m_addr; }

    // Local cell accessors (no bounds wrap)
    WoxelCell get(std::int32_t i_theta, std::int32_t i_z, std::int32_t i_r) const
    {
        return m_cells[static_cast<std::size_t>(linear_index(i_theta, i_z, i_r))];
    }

    void set(std::int32_t i_theta, std::int32_t i_z, std::int32_t i_r, WoxelCell v)
    {
        m_cells[static_cast<std::size_t>(linear_index(i_theta, i_z, i_r))] = v;
    }

    // Convert from global indices to local within this chunk
    std::tuple<std::int32_t, std::int32_t, std::int32_t> local_from_global(const WoxelIndex &g) const
    {
        const std::int32_t i_theta_local = ((g.i_theta % m_dims.Ctheta) + m_dims.Ctheta) % m_dims.Ctheta;
        const std::int32_t i_z_local = ((g.i_z % m_dims.Cz) + m_dims.Cz) % m_dims.Cz;
        const std::int32_t i_r_local = ((g.i_r % m_dims.Cr) + m_dims.Cr) % m_dims.Cr;
        return { i_theta_local, i_z_local, i_r_local };
    }
};
}

