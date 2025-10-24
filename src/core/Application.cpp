#include "steeplejack/Application.hpp"

#include <iostream>

namespace steeplejack {

Application::Application(AppConfig config) : config_(std::move(config)) {}

int Application::run() {
    std::cout << "Launching " << config_.name << " (" << config_.width << "x" << config_.height
              << ")" << std::endl;
    // TODO: hook up platform layer + Vulkan renderer
    return 0;
}

}  // namespace steeplejack
