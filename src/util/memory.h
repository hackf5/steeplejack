#pragma once

#include <iterator>
#include <ranges>
#include <type_traits>
#include <cstring>

namespace levin
{
template <typename TIter>
size_t total_bytes(TIter begin, TIter end)
{
    static_assert(std::contiguous_iterator<TIter>, "TIter must be a contiguous iterator");
    static_assert(
        std::is_standard_layout_v<std::decay_t<decltype(*begin)>> == true,
        "TIter must be an iterator to a standard layout type");

    return sizeof(*begin) * std::distance(begin, end);
}

size_t total_bytes(const std::ranges::contiguous_range auto &range)
{
    return total_bytes(std::begin(range), std::end(range));
}

template <typename TIter>
void *to_void_address(TIter iter)
{
    return (void*)std::to_address(iter);
}

template <typename TIter>
void copy_to(void *dst, TIter begin, TIter end)
{
    static_assert(std::contiguous_iterator<TIter>, "TIter must be a contiguous iterator");
    static_assert(
        std::is_standard_layout_v<std::decay_t<decltype(*begin)>>,
        "TIter must be an iterator to a standard layout type");

    memcpy(dst, to_void_address(begin), total_bytes(begin, end));
}

void copy_to(void *dst, const std::ranges::contiguous_range auto &range)
{
    copy_to(dst, std::begin(range), std::end(range));
}

template <typename T>
void copy_to(void *dst, const T &data)
{
    static_assert(
        std::is_standard_layout_v<std::decay_t<T>>,
        "T must be a standard layout type");

    memcpy(dst, &data, sizeof(data));
}
}
