#pragma once

#include "camera.h"
#include "glm_config.hpp"
#include "model.h"
#include "util/no_copy_or_move.h"
#include "vulkan/buffer/uniform_buffer.h"
#include "vulkan/device.h"

#include <cstdint>
#include <vulkan/vulkan.h>

namespace steeplejack
{
class Scene : public NoCopyOrMove
{
  private:
    Camera m_camera;
    Model m_model;
    struct LightsBlock
    {
        glm::vec3 ambientColor;
        float ambientIntensity;

        struct Spot
        {
            // https://stackoverflow.com/a/19957102/323316
            glm::vec3 position;  float intensity;
            glm::vec3 direction; float innerCos;
            glm::vec3 color;     float outerCos;
            glm::vec3 _pad;      float range;
        } spots[2];
    };
    UniformBuffer m_lights_buffers;
    LightsBlock m_lights;

  public:
    Scene(const Device& device) : m_camera(device), m_model(), m_lights_buffers(device, sizeof(LightsBlock)), m_lights{}
    {
        m_lights.ambientColor = glm::vec3(1.0f);
        m_lights.ambientIntensity = 0.1f;
        m_lights.spots[0].position = glm::vec3(2.0f, 2.0f, 2.0f);
        m_lights.spots[0].intensity = 2.0f;
        m_lights.spots[0].direction = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
        m_lights.spots[0].innerCos = glm::cos(glm::radians(15.0f));
        m_lights.spots[0].color = glm::vec3(1.0f);
        m_lights.spots[0].outerCos = glm::cos(glm::radians(25.0f));
        m_lights.spots[0].range = 6.0f;
        m_lights.spots[0]._pad[0] = m_lights.spots[0]._pad[1] = 0.0f;
        // Second spot default
        m_lights.spots[1].position = glm::vec3(-2.0f, 2.0f, 2.0f);
        m_lights.spots[1].intensity = 2.0f;
        m_lights.spots[1].direction = glm::normalize(glm::vec3(1.0f, -1.0f, -1.0f));
        m_lights.spots[1].innerCos = glm::cos(glm::radians(12.0f));
        m_lights.spots[1].color = glm::vec3(0.9f, 0.95f, 1.0f);
        m_lights.spots[1].outerCos = glm::cos(glm::radians(22.0f));
        m_lights.spots[1].range = 6.0f;
        m_lights.spots[1]._pad[0] = m_lights.spots[1]._pad[1] = 0.0f;
    }

    const Camera& camera() const
    {
        return m_camera;
    }
    Camera& camera()
    {
        return m_camera;
    }

    const Model& model() const
    {
        return m_model;
    }
    Model& model()
    {
        return m_model;
    }

    glm::vec3& ambient_color()
    {
        return m_lights.ambientColor;
    }
    float& ambient_intensity()
    {
        return m_lights.ambientIntensity;
    }

    // Spotlight controls
    glm::vec3 spotlight_position() const
    {
        return m_lights.spots[0].position;
    }
    void set_spotlight_position(const glm::vec3& p)
    {
        m_lights.spots[0].position = p;
    }
    glm::vec3 spotlight_direction() const
    {
        return m_lights.spots[0].direction;
    }
    void set_spotlight_direction(const glm::vec3& d)
    {
        m_lights.spots[0].direction = d;
    }
    glm::vec3 spotlight_color() const
    {
        return m_lights.spots[0].color;
    }
    void set_spotlight_color(const glm::vec3& c)
    {
        m_lights.spots[0].color = c;
    }
    float& spotlight_intensity()
    {
        return m_lights.spots[0].intensity;
    }
    float& spotlight_inner_cos()
    {
        return m_lights.spots[0].innerCos;
    }
    float& spotlight_outer_cos()
    {
        return m_lights.spots[0].outerCos;
    }
    float& spotlight_range()
    {
        return m_lights.spots[0].range;
    }

    // Access spot by index (0 or 1)
    LightsBlock::Spot& spot(size_t index)
    {
        return m_lights.spots[index];
    }

    // Single spotlight path extended to two spots via array; more can be added later.

    void flush(uint32_t frame_index)
    {
        m_camera.flush(frame_index);
        m_lights_buffers[frame_index].copy_from(m_lights);
        m_model.flush(frame_index);
    }

    void render(VkCommandBuffer command_buffer, uint32_t frame_index, GraphicsPipeline& pipeline)
    {
        // Reset descriptor writes per frame/draw sequence before rebinding camera and meshes
        pipeline.descriptor_set_layout().reset_writes();
        m_camera.bind(frame_index, pipeline);
        // Bind lights UBO at binding 7
        pipeline.descriptor_set_layout().write_uniform_buffer(m_lights_buffers[frame_index].descriptor(), 7);
        m_model.render(command_buffer, frame_index, pipeline);
    }
};

} // namespace steeplejack
