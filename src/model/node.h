#pragma once

#include "mesh.h"
#include "util/no_copy_or_move.h"
#include "vulkan/graphics_pipeline.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>

namespace steeplejack
{
class Node : NoCopyOrMove
{
  private:
    Node* m_parent;
    std::vector<std::unique_ptr<Node>> m_children;
    std::unique_ptr<Mesh> m_mesh;

    glm::vec3 m_translation{};
    glm::vec3 m_scale{1.0f};
    glm::quat m_rotation{};
    glm::mat4 m_matrix{};
    bool m_matrix_dirty = true;

  public:
    Node(Node* parent = nullptr, std::unique_ptr<Mesh> mesh = nullptr) : m_parent(parent), m_mesh(std::move(mesh)) {}

    Node(const Node&) = delete;

    Node* parent() const
    {
        return m_parent;
    }

    std::vector<std::unique_ptr<Node>>& children()
    {
        return m_children;
    }

    Node& add_child(std::unique_ptr<Mesh> mesh = nullptr)
    {
        auto& child = m_children.emplace_back(std::make_unique<Node>(this, std::move(mesh)));

        return *child;
    }

    const glm::vec3& translation() const
    {
        return m_translation;
    }
    glm::vec3& translation()
    {
        m_matrix_dirty = true;
        return m_translation;
    }

    const glm::vec3& scale() const
    {
        return m_scale;
    }
    glm::vec3& scale()
    {
        m_matrix_dirty = true;
        return m_scale;
    }

    const glm::quat& rotation() const
    {
        return m_rotation;
    }
    glm::quat& rotation()
    {
        m_matrix_dirty = true;
        return m_rotation;
    }

    const glm::mat4& local_matrix()
    {
        if (m_matrix_dirty)
        {
            m_matrix = glm::translate(glm::mat4(1.0f), m_translation) * glm::mat4_cast(m_rotation) *
                       glm::scale(glm::mat4(1.0f), m_scale);
            m_matrix_dirty = false;
        }

        return m_matrix;
    }

    glm::mat4 global_matrix()
    {
        return m_parent ? m_parent->global_matrix() * local_matrix() : local_matrix();
    }

    void flush(uint32_t frame_index)
    {
        if (m_mesh)
        {
            m_mesh->model() = global_matrix();
            m_mesh->flush(frame_index);
        }

        for (auto& child : m_children)
        {
            child->flush(frame_index);
        }
    }

    void render(VkCommandBuffer command_buffer, uint32_t frame_index, GraphicsPipeline& pipeline)
    {
        if (m_mesh)
        {
            m_mesh->render(command_buffer, frame_index, pipeline);
        }

        for (auto& child : m_children)
        {
            child->render(command_buffer, frame_index, pipeline);
        }
    }
};
} // namespace steeplejack