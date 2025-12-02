#include "scenes/minimal_scene_runner.h"

#include "ecs/components.h"
#include "renderer/backend.h"
#include "renderer/scene_backend_builder.h"

#include <array>
#include <glm/gtc/matrix_transform.hpp>
#include <utility>
#include <vulkan/vulkan.h>

using namespace steeplejack;

namespace
{
struct MinimalVertex
{
    glm::vec3 pos;
    glm::vec3 pad; // filler to align UV at offset 24
    glm::vec2 uv;
};

struct CameraBlock
{
    glm::mat4 proj;
    glm::mat4 view;
};

struct ModelBlock
{
    glm::mat4 model;
};
} // namespace

MinimalSceneRunner::MinimalSceneRunner(renderer::Backend& backend) : SceneRunner(backend) {}

void MinimalSceneRunner::load_resources()
{
    const auto& device = backend().device();

    // Record ECS resources
    m_scene.resources.device = &device;
    m_scene.resources.graphics_buffers = &backend().graphics_buffers();
    m_scene.resources.material_factory = &backend().material_factory();

    // Minimal geometry: single triangle (non-indexed)
    const std::array<MinimalVertex, 3> vertices{{
        {.pos = {0.0F, -0.5F, 0.0F}, .pad = {0.0F, 0.0F, 0.0F}, .uv = {0.5F, 0.0F}},
        {.pos = {-0.5F, 0.5F, 0.0F}, .pad = {0.0F, 0.0F, 0.0F}, .uv = {0.0F, 1.0F}},
        {.pos = {0.5F, 0.5F, 0.0F}, .pad = {0.0F, 0.0F, 0.0F}, .uv = {1.0F, 1.0F}},
    }};

    m_vertex_buffer = std::make_unique<BufferGPU>(
        device,
        backend().adhoc_queues(),
        static_cast<VkDeviceSize>(sizeof(vertices)),
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    m_vertex_buffer->copy_from(vertices);

    m_camera_ubo = std::make_unique<UniformBuffer>(device, sizeof(CameraBlock));
    m_model_ubo = std::make_unique<UniformBuffer>(device, sizeof(ModelBlock));

    // ECS entities: camera + single triangle with transform
    m_camera = m_scene.registry.create();
    auto& cam = m_scene.registry.emplace<ecs::Camera>(m_camera);
    cam.position = glm::vec3(0.0F, 0.0F, 2.0F);
    cam.target = glm::vec3(0.0F);
    cam.clip_near = 0.1F;
    cam.clip_far = 10.0F;
    cam.fov = 60.0F;
    m_scene.registry.emplace<ecs::MainCamera>(m_camera);

    m_triangle = m_scene.registry.create();
    m_scene.registry.emplace<ecs::Transform>(m_triangle);
}

void MinimalSceneRunner::build_scene_backend()
{
    renderer::SceneBackendBuilder builder(backend());
    auto scene_backend = builder.set_graphics("minimal").build();
    set_scene_backend(std::move(scene_backend));
}

void MinimalSceneRunner::update(const renderer::FrameContext& frame)
{
    m_time += frame.delta_seconds;
}

void MinimalSceneRunner::render(const renderer::RenderContext& render_ctx)
{
    const auto frame_index = render_ctx.frame_index;
    const auto aspect = backend().swapchain().aspect_ratio();

    // Update camera matrices
    auto& cam = m_scene.registry.get<ecs::Camera>(m_camera);
    cam.aspect = aspect;
    cam.view = glm::lookAt(cam.position, cam.target, glm::vec3(0.0F, 1.0F, 0.0F));
    cam.proj = glm::perspective(glm::radians(cam.fov), cam.aspect, cam.clip_near, cam.clip_far);
    cam.proj[1][1] *= -1.0F; // Vulkan clip space

    CameraBlock camera_block{.proj = cam.proj, .view = cam.view};
    m_camera_ubo->at(frame_index).copy_from(camera_block);

    // Update model transform (simple rotation)
    auto& transform = m_scene.registry.get<ecs::Transform>(m_triangle);
    auto model = glm::mat4(1.0F);
    model = glm::rotate(model, m_time, glm::vec3(0.0F, 0.0F, 1.0F));
    transform.world = model;

    ModelBlock model_block{.model = transform.world};
    m_model_ubo->at(frame_index).copy_from(model_block);

    auto& gp = scene_backend().graphics_pipeline();
    auto& layout = gp.descriptor_set_layout();

    layout.reset_writes();
    layout.write_uniform_buffer(m_camera_ubo->at(frame_index).descriptor(), layout.binding_handle("camera"));
    layout.write_uniform_buffer(m_model_ubo->at(frame_index).descriptor(), layout.binding_handle("model"));

    const VkCommandBuffer cmd = render_ctx.command_buffer;
    auto scope = backend().render_pass().begin(cmd, render_ctx.framebuffer);

    backend().swapchain().clip(cmd);
    gp.bind(cmd);

    const VkBuffer vb = m_vertex_buffer->vk();
    const std::array<VkDeviceSize, 1> offsets = {0};
    vkCmdBindVertexBuffers(cmd, 0, 1, &vb, offsets.begin());

    gp.push_descriptor_set(cmd);

    vkCmdDraw(cmd, 3, 1, 0, 0);
}
