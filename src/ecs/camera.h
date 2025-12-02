#pragma once

# include "glm_config.hpp"

namespace steeplejack::ecs
{
enum class ProjectionType : std::uint8_t
{
    Perspective,
    Orthographic
};

struct Camera
{
    ProjectionType projection{ProjectionType::Perspective};

    float vertical_fov_degrees{60.0F};
    float aspect_ratio{1.0F};
    float near_clip{0.1F};
    float far_clip{1000.0F};

    // World-space height covered by the orthographic view; width derives from aspect.
    float ortho_height{2.0F};

    // Clip-space jitter for TAA or sub-pixel camera sampling.
    glm::vec2 jitter{0.0F, 0.0F};
};

struct CameraPose
{
    glm::vec3 position{0.0F, 0.0F, 0.0F};
    glm::quat orientation{1.0F, 0.0F, 0.0F, 0.0F};

    [[nodiscard]] glm::mat4 world_from_camera() const;
};

struct CameraMatrices
{
    glm::mat4 view{1.0F};
    glm::mat4 proj{1.0F};
    glm::mat4 view_proj{1.0F};
    glm::mat4 inv_view{1.0F};
    glm::mat4 inv_proj{1.0F};
};

// Builds camera matrices without API-specific clip adjustments (renderer can flip/offset as needed).
[[nodiscard]] CameraMatrices make_camera_matrices(const Camera& camera,
                                                  const glm::mat4& world_from_camera,
                                                  float aspect_override = 0.0F);

[[nodiscard]] CameraMatrices make_camera_matrices(const Camera& camera,
                                                  const CameraPose& pose,
                                                  float aspect_override = 0.0F);
} // namespace steeplejack::ecs
