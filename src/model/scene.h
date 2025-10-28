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
        int32_t spotCount; // std140: 4 bytes, pad to 16 with _padCount
        glm::vec3 _padCount;

        static constexpr int MAX_SPOTS = 8;
        struct Spot
        {
            glm::vec3 position;
            float intensity;
            glm::vec3 direction;
            float innerCos;
            glm::vec3 color;
            float outerCos;
            float range;
            glm::vec3 _pad;
        } spots[MAX_SPOTS];
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
        m_lights.spotCount = 1;
        m_lights._padCount = glm::vec3(0.0f);
        // Initialize first spot
        m_lights.spots[0].position = glm::vec3(2.0f, 2.0f, 2.0f);
        m_lights.spots[0].intensity = 2.0f;
        m_lights.spots[0].direction = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
        m_lights.spots[0].innerCos = glm::cos(glm::radians(15.0f));
        m_lights.spots[0].color = glm::vec3(1.0f);
        m_lights.spots[0].outerCos = glm::cos(glm::radians(25.0f));
        m_lights.spots[0].range = 6.0f;
        m_lights.spots[0]._pad = glm::vec3(0.0f);
        // Zero the rest
        for (int i = 1; i < LightsBlock::MAX_SPOTS; ++i)
        {
            m_lights.spots[i].position = glm::vec3(0.0f);
            m_lights.spots[i].intensity = 0.0f;
            m_lights.spots[i].direction = glm::vec3(0.0f, 0.0f, -1.0f);
            m_lights.spots[i].innerCos = 0.0f;
            m_lights.spots[i].color = glm::vec3(0.0f);
            m_lights.spots[i].outerCos = 0.0f;
            m_lights.spots[i].range = 0.0f;
            m_lights.spots[i]._pad = glm::vec3(0.0f);
        }
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

    // Spotlight controls (primary spot at index 0 for now)
    glm::vec3& spotlight_position() { return m_lights.spots[0].position; }
    glm::vec3& spotlight_direction() { return m_lights.spots[0].direction; }
    glm::vec3& spotlight_color() { return m_lights.spots[0].color; }
    float& spotlight_intensity() { return m_lights.spots[0].intensity; }
    float& spotlight_inner_cos() { return m_lights.spots[0].innerCos; }
    float& spotlight_outer_cos() { return m_lights.spots[0].outerCos; }
    float& spotlight_range() { return m_lights.spots[0].range; }

    // Multiple spotlights support
    int& spot_count() { return m_lights.spotCount; }
    LightsBlock::Spot& spot(size_t index) { return m_lights.spots[index]; }

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
