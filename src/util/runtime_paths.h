#pragma once

namespace steeplejack
{
// Sets the process working directory to the directory containing the executable.
// This makes relative asset lookups (e.g., "shaders/<name>.spv") resolve
// consistently regardless of how the app is launched.
void set_working_directory_to_executable();
} // namespace steeplejack
