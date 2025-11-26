// GPU UBO caches for ECS entities (camera and model)
#pragma once

#include "vulkan/buffer/uniform_buffer.h"
#include "glm_config.hpp"

#include <entt/entt.hpp>
#include <memory>
#include <unordered_map>

namespace steeplejack
{
class Device;
}

namespace steeplejack::ecs
{
struct ModelUbo
{
    struct Block
    {
        glm::mat4 model;
    };

    std::unique_ptr<UniformBuffer> buffer; // sizeof(Block)
};

class ModelUboCache
{
  private:
    std::unordered_map<entt::entity, ModelUbo> m_items;

  public:
    ModelUbo* ensure(entt::entity e, const Device& device)
    {
        auto it = m_items.find(e);
        if (it == m_items.end())
        {
            ModelUbo ubo;
            ubo.buffer = std::make_unique<UniformBuffer>(device, sizeof(ModelUbo::Block));
            it = m_items.emplace(e, std::move(ubo)).first;
        }
        return &it->second;
    }

    ModelUbo* get(entt::entity e)
    {
        auto it = m_items.find(e);
        return (it == m_items.end()) ? nullptr : &it->second;
    }

    void remove(entt::entity e)
    {
        m_items.erase(e);
    }
};

struct CameraUbo
{
    struct Block
    {
        glm::mat4 proj;
        glm::mat4 view;
    };

    std::unique_ptr<UniformBuffer> buffer; // sizeof(Block)
};

class CameraUboCache
{
  private:
    std::unordered_map<entt::entity, CameraUbo> m_items;

  public:
    CameraUbo* ensure(entt::entity e, const Device& device)
    {
        auto it = m_items.find(e);
        if (it == m_items.end())
        {
            CameraUbo ubo;
            ubo.buffer = std::make_unique<UniformBuffer>(device, sizeof(CameraUbo::Block));
            it = m_items.emplace(e, std::move(ubo)).first;
        }
        return &it->second;
    }

    CameraUbo* get(entt::entity e)
    {
        auto it = m_items.find(e);
        return (it == m_items.end()) ? nullptr : &it->second;
    }

    void remove(entt::entity e)
    {
        m_items.erase(e);
    }
};
} // namespace steeplejack::ecs
