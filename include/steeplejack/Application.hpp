#pragma once

#include <string>

namespace steeplejack {

struct AppConfig {
    std::string name{"Steeplejack"};
    int width{1280};
    int height{720};
};

class Application {
public:
    explicit Application(AppConfig config = {});

    int run();

private:
    AppConfig config_;
};

}  // namespace steeplejack
