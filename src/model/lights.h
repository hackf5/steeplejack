#pragma once

#include "glm_config.hpp"
#include "vulkan/buffer/uniform_buffer.h"
#include "vulkan/buffer/uniform_buffer_array.h"
#include "vulkan/descriptor_set_layout.h"
#include "vulkan/device.h"

#include <vulkan/vulkan.h>

namespace steeplejack
{
inline constexpr size_t kMaxSpotLights = 8;

// https://stackoverflow.com/a/19957102/323316
// these structs are std140 aligned for UBO use
struct AmbientLightUBO
{
    glm::vec3 color;
    float intensity;
};

struct SpotLightUBO
{
    glm::vec3 position;
    float intensity;
    glm::vec3 direction;
    float innerCos;
    glm::vec3 color;
    float outerCos;
    float range;
    bool enable = false;
    glm::vec2 _pad0;
};

struct LightsUBO
{

    AmbientLightUBO ambient;
    SpotLightUBO spots[kMaxSpotLights];
};

struct SpotLightMatrices
{
    glm::mat4 viewProj[kMaxSpotLights];
    glm::vec4 debugParams; // x: shadowsEnabled
};

class Lights
{
  private:
    LightsUBO m_lights;
    SpotLightMatrices m_matrices;
    UniformBufferArray<glm::mat4> m_shadow_lights_buffer;
    UniformBuffer m_lights_buffer;
    UniformBuffer m_matrices_buffer;
    uint32_t m_shadow_lights_binding;
    uint32_t m_lights_binding;
    uint32_t m_matrices_binding;

  public:
    explicit Lights(const Device& device);

    Lights(const Lights&) = delete;
    Lights& operator=(const Lights&) = delete;
    Lights(Lights&&) = delete;
    Lights& operator=(Lights&&) = delete;

    void update();

    void flush(uint32_t frame_index);

    void bind_shadow(DescriptorSetLayout& layout, uint32_t frame_index, size_t spot_index);

    void bind(DescriptorSetLayout& layout, uint32_t frame_index);

    void set_shadows_enabled(bool enabled)
    {
        m_matrices.debugParams = glm::vec4(enabled ? 1.0f : 0.0f, 0.0f, 0.0f, 0.0f);
    }

    void set_debug_mode(int mode)
    {
        m_matrices.debugParams.y = static_cast<float>(mode);
    }

    void set_debug_spot_index(int idx)
    {
        m_matrices.debugParams.z = static_cast<float>(idx);
    }

    uint32_t shadow_lights_binding() const
    {
        return m_shadow_lights_binding;
    }

    uint32_t& shadow_lights_binding()
    {
        return m_shadow_lights_binding;
    }

    uint32_t lights_binding() const
    {
        return m_lights_binding;
    }

    uint32_t& lights_binding()
    {
        return m_lights_binding;
    }

    uint32_t matrices_binding() const
    {
        return m_matrices_binding;
    }

    uint32_t& matrices_binding()
    {
        return m_matrices_binding;
    }

    glm::vec3 ambient_color() const
    {
        return m_lights.ambient.color;
    }

    glm::vec3& ambient_color()
    {
        return m_lights.ambient.color;
    }

    float ambient_intensity() const
    {
        return m_lights.ambient.intensity;
    }

    float& ambient_intensity()
    {
        return m_lights.ambient.intensity;
    }

    size_t spots_size() const
    {
        return kMaxSpotLights;
    }

    const SpotLightUBO& spot_at(size_t index) const
    {
        return m_lights.spots[index];
    }

    SpotLightUBO& spot_at(size_t index)
    {
        return m_lights.spots[index];
    }

    const SpotLightMatrices& matrices() const
    {
        return m_matrices;
    }

    SpotLightMatrices& matrices()
    {
        return m_matrices;
    }
};
} // namespace steeplejack
