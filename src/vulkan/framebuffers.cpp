#include "framebuffers.h"

#include "depth_buffer.h"
#include "render_pass.h"
#include "spdlog/spdlog.h"
#include "swapchain.h"

#include <array>
#include <stdexcept>

using namespace steeplejack;

namespace
{
std::vector<VkFramebuffer> create_framebuffers(
    const Device& device,
    const Multisampler& multisampler,
    const Swapchain& swapchain,
    const RenderPass& render_pass,
    const DepthBuffer& depth_buffer)
{
    spdlog::info("Creating Framebuffers");

    std::vector<VkFramebuffer> framebuffers;
    framebuffers.resize(swapchain.image_count());

    for (size_t i = 0; i < swapchain.image_count(); i++)
    {
        auto attachments = std::array<VkImageView, 3>{
            multisampler.image_view(),
            depth_buffer.image_view(),
            swapchain.image_view(i),
        };

        VkFramebufferCreateInfo framebuffer_info = {};
        framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_info.renderPass = render_pass.vk();
        framebuffer_info.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebuffer_info.pAttachments = attachments.data();
        framebuffer_info.width = swapchain.extent().width;
        framebuffer_info.height = swapchain.extent().height;
        framebuffer_info.layers = 1;

        VkFramebuffer framebuffer = nullptr;
        if (vkCreateFramebuffer(device.vk(), &framebuffer_info, nullptr, &framebuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create framebuffer");
        }

        framebuffers[i] = framebuffer;
    }

    return framebuffers;
}
} // namespace

Framebuffers::~Framebuffers()
{
    spdlog::info("Destroying Framebuffers");
    for (auto* framebuffer : m_framebuffers)
    {
        vkDestroyFramebuffer(m_device.vk(), framebuffer, nullptr);
    }
}

Framebuffers::Framebuffers(
    const Device& device, const Swapchain& swapchain, const RenderPass& render_pass, const DepthBuffer& depth_buffer) :
    m_device(device),
    m_multisampler(device, swapchain),
    m_framebuffers(create_framebuffers(device, m_multisampler, swapchain, render_pass, depth_buffer))
{
}

VkFramebuffer Framebuffers::at(uint32_t image_index) const
{
    return m_framebuffers[image_index];
}
