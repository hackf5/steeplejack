#pragma once

#include <ranges>

#include "buffer_host.h"

namespace levin
{
    class StagingBuffer: public BufferHost
    {
    public:
        template <typename TIter>
        StagingBuffer(
            const Device &device,
            TIter begin,
            TIter end):
            BufferHost(
                device,
                total_bytes(begin, end),
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
        {
            copy_from(begin, end);
        }

        StagingBuffer(
            const Device &device,
            const std::ranges::contiguous_range auto &range):
            BufferHost(
                device,
                total_bytes(range),
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
        {
            copy_from(range);
        }
    };
}