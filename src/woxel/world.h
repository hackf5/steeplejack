#pragma once

#include "woxel/chunk.h"
#include "woxel/woxel.h"

#include <cstdint>
#include <memory>
#include <unordered_map>

namespace steeplejack
{
class WoxelWorld
{
  private:
    WoxelSpaceSpec m_spec;
    ChunkDims m_chunk_dims;

    // Expect sectors divisible by Ctheta for simple theta chunk wrap in PoC
    std::int32_t m_theta_chunks = 0;

    std::unordered_map<ChunkAddress, std::unique_ptr<WoxelChunk>, ChunkAddressHasher> m_chunks;

    static std::int32_t div_floor(std::int32_t a, std::int32_t b)
    {
        // floor division for negatives
        std::int32_t q = a / b;
        std::int32_t r = a % b;
        if ((r != 0) && ((r > 0) != (b > 0)))
            --q;
        return q;
    }

  public:
    WoxelWorld(WoxelSpaceSpec spec, ChunkDims dims) : m_spec(spec), m_chunk_dims(dims)
    {
        m_spec.finalize();
        m_theta_chunks = (m_spec.sectors + m_chunk_dims.Ctheta - 1) / m_chunk_dims.Ctheta;
    }

    const WoxelSpaceSpec& spec() const
    {
        return m_spec;
    }
    const ChunkDims& chunk_dims() const
    {
        return m_chunk_dims;
    }

    ChunkAddress chunk_of(const WoxelIndex& g) const
    {
        ChunkAddress a;
        a.ctheta = div_floor(g.i_theta, m_chunk_dims.Ctheta);
        a.cz = div_floor(g.i_z, m_chunk_dims.Cz);
        a.cr = div_floor(g.i_r, m_chunk_dims.Cr);

        // Wrap theta chunk ring
        if (m_theta_chunks > 0)
        {
            a.ctheta = ((a.ctheta % m_theta_chunks) + m_theta_chunks) % m_theta_chunks;
        }
        return a;
    }

    WoxelIndex index_of(const WoxelCoord& c) const
    {
        return {i_theta_of(m_spec, c.theta), i_z_of(m_spec, c.z), i_r_of(m_spec, c.r)};
    }

    WoxelChunk& get_or_create(const ChunkAddress& addr)
    {
        auto it = m_chunks.find(addr);
        if (it != m_chunks.end())
            return *it->second;
        auto chunk = std::make_unique<WoxelChunk>(m_spec, m_chunk_dims, addr);
        auto* ptr = chunk.get();
        m_chunks.emplace(addr, std::move(chunk));
        return *ptr;
    }
};
} // namespace steeplejack
