#pragma once

#include "camera.h"
#include "glm_config.hpp"
#include "model.h"
#include "model/lights.h"
#include "util/no_copy_or_move.h"
#include "vulkan/buffer/uniform_buffer.h"
#include "vulkan/device.h"
#include "vulkan/graphics_pipeline.h"

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
    Lights m_lights;

  public:
    Scene(const Device& device) : m_camera(device), m_model(), m_lights(device)
    {
        // First spot default
        m_lights.spot_at(0).enable = true;
        m_lights.spot_at(0).position = glm::vec3(2.0f, 2.0f, 2.0f);
        m_lights.spot_at(0).direction = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));

        // Second spot default
        m_lights.spot_at(1).enable = true;
        m_lights.spot_at(1).position = glm::vec3(-2.0f, 2.0f, 2.0f);
        m_lights.spot_at(1).direction = glm::normalize(glm::vec3(1.0f, -1.0f, -1.0f));

        m_lights.lights_binding() = 7;
        m_lights.matrices_binding() = 9;
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

    const Lights& lights() const
    {
        return m_lights;
    }

    Lights& lights()
    {
        return m_lights;
    }

    void flush(uint32_t frame_index)
    {
        m_camera.flush(frame_index);
        m_lights.flush(frame_index);
        m_model.flush(frame_index);
    }

    void render_shadow(VkCommandBuffer command_buffer, uint32_t frame_index, ShadowPipeline& pipeline, size_t spot_index)
    {
        pipeline.descriptor_set_layout().reset_writes();

        m_lights.bind_shadow(pipeline.descriptor_set_layout(), frame_index, spot_index);

        m_model.render_shadow(command_buffer, frame_index, pipeline, spot_index);
    }

    void render(
        VkCommandBuffer command_buffer,
        uint32_t frame_index,
        GraphicsPipeline& pipeline,
        VkDescriptorImageInfo* shadow = nullptr)
    {
        pipeline.descriptor_set_layout().reset_writes();

        m_camera.bind(frame_index, pipeline);
        m_lights.bind(pipeline.descriptor_set_layout(), frame_index);

        if (shadow != nullptr)
        {
            // Shadow map array sampler at binding 8
            pipeline.descriptor_set_layout().write_combined_image_sampler(shadow, 8);
        }

        m_model.render(command_buffer, frame_index, pipeline);
    }
};

} // namespace steeplejack
