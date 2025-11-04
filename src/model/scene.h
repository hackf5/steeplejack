#pragma once

#include "camera.h"
#include "glm_config.hpp"
#include "model.h"
#include "util/no_copy_or_move.h"
#include "vulkan/buffer/uniform_buffer.h"
#include "vulkan/device.h"

#include <cstdint>
#include <span>
#include <vulkan/vulkan.h>

namespace steeplejack
{
class Scene : public NoCopyOrMove
{
  private:
    Camera m_camera;
    Model m_model;
    struct SceneLights
    {
        constexpr static size_t kMaxSpots = 8;

        glm::vec3 ambientColor;
        float ambientIntensity;

        struct Spot
        {
            // https://stackoverflow.com/a/19957102/323316
            glm::vec3 position;
            float intensity;
            glm::vec3 direction;
            float innerCos;
            glm::vec3 color;
            float outerCos;
            float range;
            bool enable = false;
            glm::vec2 _pad0;
        } spots[kMaxSpots];
    };

    struct ShadowMatrices
    {
        glm::mat4 lightViewProj[SceneLights::kMaxSpots];
    } m_shadow_matrices;

    UniformBuffer m_lights_buffers;
    UniformBuffer m_shadow_matrices_buffers;
    SceneLights m_lights;

  public:
    Scene(const Device& device) : m_camera(device), m_model(), m_lights_buffers(device, sizeof(SceneLights)), m_shadow_matrices_buffers(device, sizeof(ShadowMatrices)), m_lights{}
    {
        m_lights.ambientColor = glm::vec3(1.0f);
        m_lights.ambientIntensity = 0.1f;

        // First spot default
        m_lights.spots[0].enable = true;
        m_lights.spots[0].position = glm::vec3(2.0f, 2.0f, 2.0f);
        m_lights.spots[0].intensity = 2.0f;
        m_lights.spots[0].direction = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
        m_lights.spots[0].innerCos = glm::cos(glm::radians(15.0f));
        m_lights.spots[0].color = glm::vec3(1.0f);
        m_lights.spots[0].outerCos = glm::cos(glm::radians(25.0f));
        m_lights.spots[0].range = 6.0f;

        // Second spot default
        m_lights.spots[1].enable = true;
        m_lights.spots[1].position = glm::vec3(-2.0f, 2.0f, 2.0f);
        m_lights.spots[1].intensity = 2.0f;
        m_lights.spots[1].direction = glm::normalize(glm::vec3(1.0f, -1.0f, -1.0f));
        m_lights.spots[1].innerCos = glm::cos(glm::radians(12.0f));
        m_lights.spots[1].color = glm::vec3(0.9f, 0.95f, 1.0f);
        m_lights.spots[1].outerCos = glm::cos(glm::radians(22.0f));
        m_lights.spots[1].range = 6.0f;
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

    size_t spots_size() const
    {
        return SceneLights::kMaxSpots;
    }

    const SceneLights::Spot& spot_at(size_t index) const
    {
        return m_lights.spots[index];
    }

    SceneLights::Spot& spot_at(size_t index)
    {
        return m_lights.spots[index];
    }

    // Single spotlight path extended to two spots via array; more can be added later.

    void flush(uint32_t frame_index)
    {
        m_camera.flush(frame_index);
        m_lights_buffers[frame_index].copy_from(m_lights);
        m_shadow_matrices_buffers[frame_index].copy_from(m_shadow_matrices);
        m_model.flush(frame_index);
    }

    void render(VkCommandBuffer command_buffer, uint32_t frame_index, GraphicsPipeline& pipeline)
    {
        // Reset descriptor writes per frame/draw sequence before rebinding camera and meshes
        pipeline.descriptor_set_layout().reset_writes();
        m_camera.bind(frame_index, pipeline);

        // Bind lights UBO at binding 7
        pipeline.descriptor_set_layout().write_uniform_buffer(m_lights_buffers[frame_index].descriptor(), 7);

        // Bind shadow matrices UBO at binding 9
        pipeline.descriptor_set_layout().write_uniform_buffer(m_shadow_matrices_buffers[frame_index].descriptor(), 9);

        m_model.render(command_buffer, frame_index, pipeline);
    }
};

} // namespace steeplejack
