#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>

namespace steeplejack
{
class AdhocQueues;
class Device;
class Sampler;
class Texture;
enum class TextureColorSpace : std::uint8_t;

class TextureFactory
{
  private:
    const Device& m_device;
    const Sampler& m_sampler;
    const AdhocQueues& m_adhoc_queues;

    std::unordered_map<std::string, std::unique_ptr<Texture>> m_textures;

  public:
    TextureFactory(const Device& device, const Sampler& sampler, const AdhocQueues& adhoc_queues);

    TextureFactory(const TextureFactory&) = delete;
    TextureFactory& operator=(const TextureFactory&) = delete;
    TextureFactory(TextureFactory&&) = delete;
    TextureFactory& operator=(TextureFactory&&) = delete;
    ~TextureFactory() = default;

    void load(const std::string& name, const std::string& texture_name, TextureColorSpace color_space);

    [[nodiscard]] Texture* at(const std::string& name);

    bool contains(const std::string& name) const;

    void remove(const std::string& name);

    void clear();

    // Ensure a 1x1 RGBA texture exists with given color
    void ensure_texture_rgba_1x1(
        const std::string& name, uint8_t r, uint8_t g, uint8_t b, uint8_t a, TextureColorSpace color_space);
};
} // namespace steeplejack
