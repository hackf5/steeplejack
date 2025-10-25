add_library(stb STATIC ${CMAKE_CURRENT_LIST_DIR}/stb/stb.cpp)
target_include_directories(stb PUBLIC ${CMAKE_CURRENT_LIST_DIR}/stb)