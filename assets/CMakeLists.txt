add_custom_target(
    assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/textures ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/textures
)

add_dependencies(${PROJECT_NAME} assets)