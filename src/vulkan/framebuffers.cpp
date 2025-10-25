#include "vulkan/framebuffers.h"

#include <array>
#include <stdexcept>

#include "spdlog/spdlog.h"

using namespace steeplejack;

Framebuffers::Framebuffers(const Device& device,
                           const Swapchain& swapchain,
                           const RenderPass& render_pass,
                           const DepthBuffer& depth_buffer)
    : device_(device),
      multisampler_(device, swapchain),
      framebuffers_(create_framebuffers(swapchain, render_pass, depth_buffer)) {}

Framebuffers::~Framebuffers() {
    for (auto fb : framebuffers_) {
        vkDestroyFramebuffer(device_, fb, nullptr);
    }
}

std::vector<VkFramebuffer> Framebuffers::create_framebuffers(const Swapchain& swapchain,
                                                             const RenderPass& render_pass,
                                                             const DepthBuffer& depth_buffer) {
    std::vector<VkFramebuffer> fbs;
    fbs.resize(swapchain.image_count());
    for (size_t i = 0; i < swapchain.image_count(); i++) {
        auto attachments = std::array<VkImageView, 3>{multisampler_.image_view(), depth_buffer.image_view(),
                                                      swapchain.image_view(i)};

        VkFramebufferCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        ci.renderPass = render_pass;
        ci.attachmentCount = static_cast<uint32_t>(attachments.size());
        ci.pAttachments = attachments.data();
        ci.width = swapchain.extent().width;
        ci.height = swapchain.extent().height;
        ci.layers = 1;

        VkFramebuffer fb{};
        if (vkCreateFramebuffer(device_, &ci, nullptr, &fb) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create framebuffer");
        }
        fbs[i] = fb;
    }
    return fbs;
}

