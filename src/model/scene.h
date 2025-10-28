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
    struct AmbientBlock
    {
        glm::vec3 color;
        float intensity;
    };
    UniformBuffer m_ambient_buffers;
    AmbientBlock m_ambient_block;

  public:
    Scene(const Device& device) :
        m_camera(device),
        m_model(),
        m_ambient_buffers(device, sizeof(AmbientBlock)),
        m_ambient_block({glm::vec3(1.0f), 0.2f})
    {
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
        return m_ambient_block.color;
    }
    float& ambient_intensity()
    {
        return m_ambient_block.intensity;
    }

    void flush(uint32_t frame_index)
    {
        m_camera.flush(frame_index);
        m_ambient_buffers[frame_index].copy_from(m_ambient_block);
        m_model.flush(frame_index);
    }

    void render(VkCommandBuffer command_buffer, uint32_t frame_index, GraphicsPipeline& pipeline)
    {
        // Reset descriptor writes per frame/draw sequence before rebinding camera and meshes
        pipeline.descriptor_set_layout().reset_writes();
        m_camera.bind(frame_index, pipeline);
        // Bind ambient light UBO at binding 7
        pipeline.descriptor_set_layout().write_uniform_buffer(m_ambient_buffers[frame_index].descriptor(), 7);
        m_model.render(command_buffer, frame_index, pipeline);
    }
};

} // namespace steeplejack