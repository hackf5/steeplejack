#include "model/lights.h"

using namespace steeplejack;

Lights::Lights(const Device& device) :
    m_lights{},
    m_matrices{},
    m_shadow_lights_buffer(device, kMaxSpotLights),
    m_lights_buffer(device, sizeof(LightsUBO)),
    m_matrices_buffer(device, sizeof(SpotLightMatrices)),
    m_shadow_lights_binding(0),
    m_lights_binding(0),
    m_matrices_binding(1)
{
    // Initialize ambient light to white with low intensity
    m_lights.ambient.color = glm::vec3(1.0f);
    m_lights.ambient.intensity = 0.1f;

    // Initialize spotlights to disabled state with default parameters
    for (auto& spot : m_lights.spots)
    {
        spot = {};
        spot.enable = false;
        spot.color = glm::vec3(1.0f);
        spot.intensity = 1.0f;
        spot.range = 6.0f;
        spot.innerCos = glm::cos(glm::radians(15.0f));
        spot.outerCos = glm::cos(glm::radians(25.0f));
        spot.position = glm::vec3(0.0f);
        spot.direction = glm::vec3(0.0f);
    }
}

void Lights::update()
{
    for (size_t i = 0; i < kMaxSpotLights; ++i)
    {
        const auto& spot = m_lights.spots[i];
        if (!spot.enable)
            continue;
        auto center = spot.position + glm::normalize(spot.direction);
        auto up = glm::vec3(0.0f, 0.0f, 1.0f);
        if (glm::abs(glm::dot(up, spot.direction)) > 0.99f)
        {
            up = glm::vec3(0.0f, 1.0f, 0.0f);
        }
        auto view = glm::lookAt(spot.position, center, up);
        auto fovY = glm::acos(spot.outerCos) * 2.0f;
        // Choose a near plane that balances precision and clipping
        auto near = glm::max(0.05f, 0.02f * spot.range);
        auto proj = glm::perspective(fovY, 1.0f, near, spot.range);
        proj[1][1] *= -1;
        m_matrices.viewProj[i] = proj * view;
    }
}

void Lights::flush(uint32_t frame_index)
{
    // Update shadow lights UBO
    for (size_t i = 0; i < kMaxSpotLights; ++i)
    {
        if (!m_lights.spots[i].enable)
            continue;
        m_shadow_lights_buffer.copy_from_at(m_matrices.viewProj[i], i, frame_index);
    }

    // Update lights UBO
    auto& light_buffer = m_lights_buffer[frame_index];
    light_buffer.copy_from(m_lights);

    // Update spotlight matrices UBO
    auto& matrix_buffer = m_matrices_buffer[frame_index];
    matrix_buffer.copy_from(m_matrices);
}

void Lights::bind_shadow(DescriptorSetLayout& layout, uint32_t frame_index, size_t spot_index)
{
    // Bind shadow lights UBO
    auto* shadow_buffer = m_shadow_lights_buffer.descriptor_ptr_at(spot_index, frame_index);
    layout.write_uniform_buffer(shadow_buffer, m_shadow_lights_binding);
}

void Lights::bind(DescriptorSetLayout& layout, uint32_t frame_index)
{
    // Bind lights UBO
    auto& light_buffer = m_lights_buffer[frame_index];
    layout.write_uniform_buffer(light_buffer.descriptor(), m_lights_binding);

    // Bind spotlight matrices UBO
    auto& matrix_buffer = m_matrices_buffer[frame_index];
    layout.write_uniform_buffer(matrix_buffer.descriptor(), m_matrices_binding);
}
