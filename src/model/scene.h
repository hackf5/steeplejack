#pragma once

#include "camera.h"
#include "model.h"
#include "util/no_copy_or_move.h"
#include "vulkan/buffer/uniform_buffer.h"
#include "vulkan/device.h"
#include <glm/glm.hpp>

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
        glm::vec3 spotPosition;
        float spotIntensity;
        glm::vec3 spotDirection;
        float spotInnerCos;
        glm::vec3 spotColor;
        float spotOuterCos;
        float spotRange;
        glm::vec3 _pad0;
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
        m_lights.spotPosition = glm::vec3(2.0f, 2.0f, 2.0f);
        m_lights.spotIntensity = 2.0f;
        m_lights.spotDirection = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
        m_lights.spotInnerCos = glm::cos(glm::radians(15.0f));
        m_lights.spotColor = glm::vec3(1.0f);
        m_lights.spotOuterCos = glm::cos(glm::radians(25.0f));
        m_lights.spotRange = 6.0f;
        m_lights._pad0 = glm::vec3(0.0f);
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
    glm::vec3& spotlight_position() { return m_lights.spotPosition; }
    glm::vec3& spotlight_direction() { return m_lights.spotDirection; }
    glm::vec3& spotlight_color() { return m_lights.spotColor; }
    float& spotlight_intensity() { return m_lights.spotIntensity; }
    float& spotlight_inner_cos() { return m_lights.spotInnerCos; }
    float& spotlight_outer_cos() { return m_lights.spotOuterCos; }
    float& spotlight_range() { return m_lights.spotRange; }

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
