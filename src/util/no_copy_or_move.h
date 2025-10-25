#pragma once

namespace steeplejack
{
class NoCopyOrMove
{
public:
    NoCopyOrMove(const NoCopyOrMove &) = delete;
    NoCopyOrMove &operator=(const NoCopyOrMove &) = delete;
    NoCopyOrMove(NoCopyOrMove &&) = delete;
    NoCopyOrMove &operator=(NoCopyOrMove &&) = delete;

protected:
    NoCopyOrMove() = default;
    ~NoCopyOrMove() = default;
};
}