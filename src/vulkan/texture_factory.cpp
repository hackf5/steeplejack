#include "vulkan/texture_factory.h"

#include "vulkan/adhoc_queues.h"
#include "vulkan/device.h"
#include "vulkan/sampler.h"
#include "vulkan/texture.h"

#include <array>
#include <format>

using namespace steeplejack;

TextureFactory::TextureFactory(const Device& device, const Sampler& sampler, const AdhocQueues& adhoc_queues) :
    m_device(device), m_sampler(sampler), m_adhoc_queues(adhoc_queues)
{
}

void TextureFactory::load(const std::string& name, const std::string& texture_name, TextureColorSpace color_space)
{
    m_textures[name] = std::make_unique<Texture>(m_device, m_sampler, m_adhoc_queues, texture_name, color_space);
}

void TextureFactory::remove(const std::string& name)
{
    m_textures.erase(name);
}

void TextureFactory::clear()
{
    m_textures.clear();
}

Texture* TextureFactory::at(const std::string& name)
{
    auto it = m_textures.find(name);
    if (it == m_textures.end())
    {
        throw std::runtime_error(std::format("Texture {} not found", name));
    }

    return it->second.get();
}

bool TextureFactory::contains(const std::string& name) const
{
    return m_textures.contains(name);
}

void TextureFactory::ensure_texture_rgba_1x1(
    const std::string& name, uint8_t r, uint8_t g, uint8_t b, uint8_t a, TextureColorSpace color_space)
{
    if (contains(name))
    {
        return;
    }

    const std::array<std::byte, 4> px{std::byte{r}, std::byte{g}, std::byte{b}, std::byte{a}};
    m_textures[name] = std::make_unique<Texture>(
        m_device,
        m_sampler,
        m_adhoc_queues,
        1,
        1,
        color_space,
        std::span<const std::byte>(px.data(), px.size()));
}
