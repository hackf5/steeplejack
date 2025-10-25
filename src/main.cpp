#include "steeplejack/Application.hpp"

int main() {
    steeplejack::AppConfig config;
    config.name = "Steeplejack Prototype";
    config.width = 1600;
    config.height = 900;

    steeplejack::Application app(config);
    return app.run();
}
