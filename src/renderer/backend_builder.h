#pragma once

#include <memory>
#include <string>

namespace steeplejack::renderer
{
class Backend;

// Builder for the global render backend.
class BackendBuilder
{
  private:
    struct BackendConfig
    {
        int window_width{1280};
        int window_height{720};
        std::string window_title{"Steeplejack"};
        bool enable_validation{false};
    };

    BackendConfig m_config{};

  public:
    BackendBuilder& set_window(int width, int height, std::string title);
    BackendBuilder& set_validation(bool enable);

    std::unique_ptr<Backend> build();
};
} // namespace steeplejack::renderer

