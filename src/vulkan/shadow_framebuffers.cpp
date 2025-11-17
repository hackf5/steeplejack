#include "shadow_framebuffers.h"

#include "spdlog/spdlog.h"

#include <stdexcept>

using namespace steeplejack;

ShadowFramebuffers::ShadowFramebuffers(
    const Device& device, const ShadowMapArray& shadow_map_array, const ShadowRenderPass& shadow_render_pass) :
    m_device(device), m_framebuffers(create_framebuffers(shadow_map_array, shadow_render_pass))
{
}

ShadowFramebuffers::~ShadowFramebuffers()
{
    spdlog::info("Destroying Shadow Framebuffers");
    for (auto* framebuffer : m_framebuffers)
    {
        vkDestroyFramebuffer(m_device.vk(), framebuffer, nullptr);
    }
}

std::vector<VkFramebuffer> ShadowFramebuffers::create_framebuffers(
    const ShadowMapArray& shadow_map_array, const ShadowRenderPass& shadow_render_pass)
{
    spdlog::info("Creating Shadow Framebuffers");

    std::vector<VkFramebuffer> framebuffers;
    framebuffers.resize(shadow_map_array.layers());

    for (size_t i = 0; i < shadow_map_array.layers(); ++i)
    {
        auto* attachment = shadow_map_array.layer_view(i);

        VkFramebufferCreateInfo framebuffer_info = {};
        framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_info.renderPass = shadow_render_pass;
        framebuffer_info.attachmentCount = 1;
        framebuffer_info.pAttachments = &attachment;
        framebuffer_info.width = shadow_map_array.resolution();
        framebuffer_info.height = shadow_map_array.resolution();
        framebuffer_info.layers = 1;

        VkFramebuffer framebuffer = nullptr;
        if (vkCreateFramebuffer(m_device.vk(), &framebuffer_info, nullptr, &framebuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create shadow framebuffer");
        }

        framebuffers[i] = framebuffer;
    }

    return framebuffers;
}