#pragma once

#include "glm_config.hpp"
#include "vulkan/buffer/uniform_buffer.h"
#include "vulkan/device.h"
#include "vulkan/graphics_pipeline.h"

#include <vulkan/vulkan.h>

namespace steeplejack
{
class Camera
{
  private:
    struct UniformBlock
    {
        glm::mat4 proj;
        glm::mat4 view;
    };

    UniformBuffer m_uniform_buffers;

    glm::vec3 m_position;
    glm::vec3 m_target;

    float m_fov{45.0F};
    float m_aspect_ratio{1.0F};
    float m_near{0.1F};
    float m_far{10.0F};
    bool m_dirty = true;

    UniformBlock m_uniform_block;
    const DescriptorSetLayout* m_cached_layout = nullptr;
    DescriptorSetLayout::BindingHandle m_camera_handle{};

    void update()
    {
        if (!m_dirty)
        {
            return;
        }

        m_uniform_block.proj = glm::perspective(glm::radians(m_fov), m_aspect_ratio, m_near, m_far);
        m_uniform_block.proj[1][1] *= -1;

        m_uniform_block.view = glm::lookAt(m_position, m_target, glm::vec3(0.0F, 0.0F, 1.0F));

        m_dirty = false;
    }

  public:
    explicit Camera(const Device& device) :
        m_uniform_buffers(device, sizeof(UniformBlock)),
        m_position(glm::zero<glm::vec3>()),
        m_target(glm::zero<glm::vec3>()),
        m_uniform_block({.proj = glm::identity<glm::mat4>(), .view = glm::identity<glm::mat4>()})
    {
    }

    Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;
    Camera(Camera&&) = delete;
    Camera& operator=(Camera&&) = delete;
    ~Camera() = default;

    [[nodiscard]] const glm::vec3& position() const
    {
        return m_position;
    }
    glm::vec3& position()
    {
        m_dirty = true;
        return m_position;
    }

    const glm::vec3& target() const
    {
        return m_target;
    }
    glm::vec3& target()
    {
        m_dirty = true;
        return m_target;
    }

    float fov() const
    {
        return m_fov;
    }
    float& fov()
    {
        m_dirty = true;
        return m_fov;
    }

    float aspect_ratio() const
    {
        return m_aspect_ratio;
    }
    float& aspect_ratio()
    {
        m_dirty = true;
        return m_aspect_ratio;
    }

    float clip_near() const
    {
        return m_near;
    }
    float& clip_near()
    {
        m_dirty = true;
        return m_near;
    }

    float clip_far() const
    {
        return m_far;
    }
    float& clip_far()
    {
        m_dirty = true;
        return m_far;
    }

    const glm::mat4& view()
    {
        update();
        return m_uniform_block.view;
    }
    const glm::mat4& proj()
    {
        update();
        return m_uniform_block.proj;
    }

    void flush(uint32_t frame_index)
    {
        update();
        m_uniform_buffers[frame_index].copy_from(m_uniform_block);
    }

    void bind(uint32_t frame_index, GraphicsPipeline& pipeline)
    {
        auto& layout = pipeline.descriptor_set_layout();
        if (m_cached_layout != &layout)
        {
            m_cached_layout = &layout;
            m_camera_handle = layout.binding_handle("camera");
        }
        layout.write_uniform_buffer(m_uniform_buffers[frame_index].descriptor(), m_camera_handle);
    }
};
} // namespace steeplejack
