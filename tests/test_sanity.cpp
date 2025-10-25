#include <catch2/catch_test_macros.hpp>
#include "steeplejack/Application.hpp"

TEST_CASE("AppConfig defaults are sane", "[core]") {
    steeplejack::AppConfig cfg{};
    REQUIRE(cfg.name == "Steeplejack");
    REQUIRE(cfg.width == 1280);
    REQUIRE(cfg.height == 720);
}

TEST_CASE("Application runs and exits cleanly", "[core]") {
    steeplejack::Application app{};
    REQUIRE(app.run() == 0);
}