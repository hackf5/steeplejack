#include "ecs/camera.h"

namespace steeplejack::ecs
{
namespace
{
[[nodiscard]] float sanitize_positive(float value, float fallback)
{
    return value > 0.0F ? value : fallback;
}

[[nodiscard]] float sanitize_far(float near_clip, float far_clip)
{
    const float safe_far = far_clip > near_clip + 0.001F ? far_clip : near_clip + 1.0F;
    return sanitize_positive(safe_far, near_clip + 1.0F);
}

[[nodiscard]] glm::quat normalize_or_identity(const glm::quat& q)
{
    const float len2 = glm::dot(q, q);
    if (len2 <= 0.0F)
    {
        return glm::quat{1.0F, 0.0F, 0.0F, 0.0F};
    }
    return glm::normalize(q);
}
} // namespace

glm::mat4 CameraPose::world_from_camera() const
{
    const glm::quat orient = normalize_or_identity(orientation);
    const glm::mat4 rotation = glm::toMat4(orient);
    return glm::translate(glm::mat4{1.0F}, position) * rotation;
}

CameraMatrices make_camera_matrices(const Camera& camera, const glm::mat4& world_from_camera, float aspect_override)
{
    CameraMatrices matrices{};

    const float aspect = aspect_override > 0.0F ? aspect_override : camera.aspect_ratio;
    const float near_clip = sanitize_positive(camera.near_clip, 0.1F);
    const float far_clip = sanitize_far(near_clip, camera.far_clip);
    const float ortho_height = sanitize_positive(camera.ortho_height, 1.0F);

    matrices.view = glm::inverse(world_from_camera);

    if (camera.projection == ProjectionType::Orthographic)
    {
        const float half_height = ortho_height * 0.5F;
        const float half_width = half_height * aspect;
        matrices.proj = glm::ortho(-half_width, half_width, -half_height, half_height, near_clip, far_clip);
    }
    else
    {
        const float vfov_radians = glm::radians(sanitize_positive(camera.vertical_fov_degrees, 60.0F));
        const float safe_aspect = sanitize_positive(aspect, 1.0F);
        matrices.proj = glm::perspective(vfov_radians, safe_aspect, near_clip, far_clip);
    }

    if (camera.jitter.x != 0.0F || camera.jitter.y != 0.0F)
    {
        matrices.proj[2][0] += camera.jitter.x;
        matrices.proj[2][1] += camera.jitter.y;
    }

    matrices.view_proj = matrices.proj * matrices.view;
    matrices.inv_view = glm::inverse(matrices.view);
    matrices.inv_proj = glm::inverse(matrices.proj);

    return matrices;
}

CameraMatrices make_camera_matrices(const Camera& camera, const CameraPose& pose, float aspect_override)
{
    return make_camera_matrices(camera, pose.world_from_camera(), aspect_override);
}
} // namespace steeplejack::ecs
