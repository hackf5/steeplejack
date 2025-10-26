#pragma once

#include "vulkan/adhoc_queues.h"
#include "vulkan/device.h"
#include "vulkan/sampler.h"
#include "vulkan/texture.h"

#include <cstdint>
#include <format>
#include <memory>
#include <string>
#include <unordered_map>

namespace steeplejack
{
class TextureFactory
{
  private:
    const Device& m_device;
    const Sampler& m_sampler;
    const AdhocQueues& m_adhoc_queues;

    std::unordered_map<std::string, std::unique_ptr<Texture>> m_textures;

  public:
    TextureFactory(const Device& device, const Sampler& sampler, const AdhocQueues& adhoc_queues) :
        m_device(device), m_sampler(sampler), m_adhoc_queues(adhoc_queues), m_textures()
    {
    }

    void load_texture(const std::string& name, const std::string& texture_name)
    {
        m_textures[name] = std::make_unique<Texture>(m_device, m_sampler, m_adhoc_queues, texture_name);
    }

    void remove_texture(const std::string& name)
    {
        m_textures.erase(name);
    }

    void clear()
    {
        m_textures.clear();
    }

    Texture* operator[](const std::string& name)
    {
        auto it = m_textures.find(name);
        if (it == m_textures.end())
        {
            throw std::runtime_error(std::format("Texture {} not found", name));
        }

        return it->second.get();
    }
};
} // namespace steeplejack