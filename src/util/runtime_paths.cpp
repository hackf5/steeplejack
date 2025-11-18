// spellchecker: ignore wpath

#include "runtime_paths.h"

#ifdef _WIN32
#include <windows.h>
#elifdef __linux__
#include <limits.h>
#include <unistd.h>
#endif

#include "spdlog/spdlog.h"

#include <array>
#include <filesystem>

namespace steeplejack
{
static std::filesystem::path executable_directory()
{
#ifdef _WIN32
    std::array<wchar_t, MAX_PATH> wpath = {};
    DWORD const len = GetModuleFileNameW(nullptr, wpath.data(), MAX_PATH);
    if (len == 0 || len == MAX_PATH)
    {
        return std::filesystem::current_path();
    }
    std::filesystem::path const exe_path(wpath.data());
    return exe_path.parent_path();
#elifdef __linux__
    std::array<char, PATH_MAX> buf;
    ssize_t len = ::readlink("/proc/self/exe", buf.data(), buf.size());
    if (len <= 0 || static_cast<size_t>(len) >= buf.size())
    {
        return std::filesystem::current_path();
    }
    std::filesystem::path exe_path(std::string(buf.data(), static_cast<size_t>(len)));
    return exe_path.parent_path();
#else
    return std::filesystem::current_path();
#endif
}

void set_working_directory_to_executable()
{
    try
    {
        auto dir = executable_directory();
        spdlog::info("Setting working directory to executable dir: {}", dir.string());
        std::filesystem::current_path(dir);
    }
    catch (const std::exception& e)
    {
        spdlog::warn("Failed to set working directory: {}", e.what());
    }
}
} // namespace steeplejack
