#pragma once

#include "camera.h"
#include "model.h"
#include "util/no_copy_or_move.h"
#include "vulkan/buffer/uniform_buffer.h"
#include "vulkan/device.h"
#include <glm/glm.hpp>
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
        alignas(16) glm::vec3 ambientColor;
        float ambientIntensity;

        struct alignas(16) Spot
        {
            alignas(16) glm::vec3 position;
            float intensity;
            alignas(16) glm::vec3 direction;
            float innerCos;
            alignas(16) glm::vec3 color;
            float outerCos;
            float range;
            float _pad[3];
        } spot;
    };
    UniformBuffer m_lights_buffers;
    LightsBlock m_lights;

  public:
    Scene(const Device& device) :
        m_camera(device),
        m_model(),
        m_lights_buffers(device, sizeof(LightsBlock)),
        m_lights{}
    {
        m_lights.ambientColor = glm::vec3(1.0f);
        m_lights.ambientIntensity = 0.1f;
        m_lights.spot.position = glm::vec3(2.0f, 2.0f, 2.0f);
        m_lights.spot.intensity = 2.0f;
        m_lights.spot.direction = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
        m_lights.spot.innerCos = glm::cos(glm::radians(15.0f));
        m_lights.spot.color = glm::vec3(1.0f);
        m_lights.spot.outerCos = glm::cos(glm::radians(25.0f));
        m_lights.spot.range = 6.0f;
        m_lights.spot._pad[0] = m_lights.spot._pad[1] = m_lights.spot._pad[2] = 0.0f;
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
    glm::vec3 spotlight_position() const { return m_lights.spot.position; }
    void set_spotlight_position(const glm::vec3& p)
    {
        m_lights.spot.position = p;
    }
    glm::vec3 spotlight_direction() const { return m_lights.spot.direction; }
    void set_spotlight_direction(const glm::vec3& d)
    {
        m_lights.spot.direction = d;
    }
    glm::vec3 spotlight_color() const { return m_lights.spot.color; }
    void set_spotlight_color(const glm::vec3& c)
    {
        m_lights.spot.color = c;
    }
    float& spotlight_intensity() { return m_lights.spot.intensity; }
    float& spotlight_inner_cos() { return m_lights.spot.innerCos; }
    float& spotlight_outer_cos() { return m_lights.spot.outerCos; }
    float& spotlight_range() { return m_lights.spot.range; }

    // Single spotlight in this simplified path; arrays can be reintroduced later.

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
