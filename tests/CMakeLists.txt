find_package(Catch2 3 CONFIG REQUIRED)

add_executable(steeplejack_tests
  test_sanity.cpp
)

target_link_libraries(steeplejack_tests PRIVATE
  steeplejack_engine
  Catch2::Catch2WithMain
)

# Ensure tests build with the same standard/warnings
target_compile_features(steeplejack_tests PRIVATE cxx_std_23)

include(Catch)
catch_discover_tests(steeplejack_tests)
