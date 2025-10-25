find_package(OpenGL REQUIRED)
find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

set(IMGUI_DIR ${CMAKE_CURRENT_LIST_DIR}/imgui)

file(
    GLOB
    IMGUI_SOURCES
    imgui/*.cpp
    imgui/*.h
    imgui/misc/cpp/imgui_stdlib.*
    imgui/backends/imgui_impl_glfw.*
    imgui/backends/imgui_impl_vulkan.*)

add_library(imgui STATIC ${IMGUI_SOURCES})

target_link_libraries(imgui PUBLIC glfw Vulkan::Vulkan OpenGL::GL)
target_include_directories(imgui PUBLIC imgui)