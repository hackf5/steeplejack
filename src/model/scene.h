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
        glm::vec3 ambientColor;
        float ambientIntensity;
        glm::vec4 spotPosIntensity;  // xyz position, w intensity
        glm::vec4 spotDirInnerCos;   // xyz direction, w inner cos
        glm::vec4 spotColorOuterCos; // rgb color, w outer cos
        glm::vec4 spotRangePad;      // x range, yzw pad
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
        m_lights.spotPosIntensity = glm::vec4(2.0f, 2.0f, 2.0f, 2.0f);
        m_lights.spotDirInnerCos = glm::vec4(glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f)),
                                             glm::cos(glm::radians(15.0f)));
        m_lights.spotColorOuterCos = glm::vec4(1.0f, 1.0f, 1.0f, glm::cos(glm::radians(25.0f)));
        m_lights.spotRangePad = glm::vec4(6.0f, 0.0f, 0.0f, 0.0f);
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
    glm::vec3 spotlight_position() const { return glm::vec3(m_lights.spotPosIntensity); }
    void set_spotlight_position(const glm::vec3& p)
    {
        m_lights.spotPosIntensity = glm::vec4(p, m_lights.spotPosIntensity.w);
    }
    glm::vec3 spotlight_direction() const { return glm::vec3(m_lights.spotDirInnerCos); }
    void set_spotlight_direction(const glm::vec3& d)
    {
        m_lights.spotDirInnerCos = glm::vec4(d, m_lights.spotDirInnerCos.w);
    }
    glm::vec3 spotlight_color() const { return glm::vec3(m_lights.spotColorOuterCos); }
    void set_spotlight_color(const glm::vec3& c)
    {
        m_lights.spotColorOuterCos = glm::vec4(c, m_lights.spotColorOuterCos.w);
    }
    float& spotlight_intensity() { return m_lights.spotPosIntensity.w; }
    float& spotlight_inner_cos() { return m_lights.spotDirInnerCos.w; }
    float& spotlight_outer_cos() { return m_lights.spotColorOuterCos.w; }
    float& spotlight_range() { return m_lights.spotRangePad.x; }

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
