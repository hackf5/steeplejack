#pragma once

#include "model/scene.h"
#include "util/no_copy_or_move.h"
#include "vulkan/device.h"
#include "vulkan/graphics_buffers.h"
#include "vulkan/graphics_pipeline.h"
#include "vulkan/shadow_pipeline.h"
#include "vulkan/material_factory.h"

#include <chrono>
#include <string>

namespace steeplejack
{
class RenderScene : NoCopyOrMove
{
  private:
    const std::string m_vertex_shader;
    const std::string m_fragment_shader;

  protected:
    Scene m_scene;

    float time_delta()
    {
        static auto start_time = std::chrono::high_resolution_clock::now();
        auto current_time = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();
    }

    virtual void update(uint32_t frame_index, float aspect_ratio, float time) = 0;

  public:
    RenderScene(const Device& device, const std::string& vertex_shader, const std::string& fragment_shader) :
        m_vertex_shader(vertex_shader), m_fragment_shader(fragment_shader), m_scene(device)
    {
    }

    virtual ~RenderScene() = default;

    const std::string& vertex_shader() const
    {
        return m_vertex_shader;
    }
    const std::string& fragment_shader() const
    {
        return m_fragment_shader;
    }

    const Scene& scene() const
    {
        return m_scene;
    }

    virtual void load(const Device& device, MaterialFactory& material_factory, GraphicsBuffers& graphics_buffers) = 0;

    void update(uint32_t frame_index, float aspect_ratio)
    {
        auto time = time_delta();
        update(frame_index, aspect_ratio, time);
    }

    void render_shadow(VkCommandBuffer command_buffer, uint32_t frame_index, ShadowPipeline& pipeline, size_t spot_index)
    {
        m_scene.render_shadow(command_buffer, frame_index, pipeline, spot_index);
    }

    void render(
        VkCommandBuffer command_buffer,
        uint32_t frame_index,
        GraphicsPipeline& pipeline,
        VkDescriptorImageInfo* shadow = nullptr)
    {
        m_scene.render(command_buffer, frame_index, pipeline, shadow);
    }
};
} // namespace steeplejack
