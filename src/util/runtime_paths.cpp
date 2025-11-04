#include "runtime_paths.h"

#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#include <limits.h>
#include <unistd.h>
#endif

#include "spdlog/spdlog.h"

namespace steeplejack
{
static std::filesystem::path executable_directory()
{
#if defined(_WIN32)
    wchar_t wpath[MAX_PATH];
    DWORD len = GetModuleFileNameW(nullptr, wpath, MAX_PATH);
    if (len == 0 || len == MAX_PATH)
    {
        return std::filesystem::current_path();
    }
    std::filesystem::path exe_path(wpath);
    return exe_path.parent_path();
#elif defined(__linux__)
    char buf[PATH_MAX];
    ssize_t len = ::readlink("/proc/self/exe", buf, sizeof(buf));
    if (len <= 0 || static_cast<size_t>(len) >= sizeof(buf))
    {
        return std::filesystem::current_path();
    }
    std::filesystem::path exe_path(std::string(buf, static_cast<size_t>(len)));
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

