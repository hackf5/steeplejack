#include "shadow_render_pass.h"

#include "spdlog/spdlog.h"

#include <stdexcept>

using namespace steeplejack;

ShadowRenderPass::ShadowRenderPass(const Device& device, VkFormat depth_format) :
    m_device(device), m_render_pass(create_render_pass(depth_format))
{
}

ShadowRenderPass::~ShadowRenderPass()
{
    spdlog::info("Destroying Shadow Render Pass");
    vkDestroyRenderPass(m_device.vk(), m_render_pass, nullptr);
}

VkRenderPass ShadowRenderPass::create_render_pass(VkFormat depth_format) const
{
    spdlog::info("Creating Shadow Render Pass");

    VkAttachmentDescription depth_attachment = {};
    depth_attachment.format = depth_format;
    depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

    VkAttachmentReference depth_attachment_ref = {};
    depth_attachment_ref.attachment = 0;
    depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 0;
    subpass.pDepthStencilAttachment = &depth_attachment_ref;

    VkRenderPassCreateInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &depth_attachment;
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = 0;

    VkRenderPass render_pass = nullptr;
    if (vkCreateRenderPass(m_device.vk(), &render_pass_info, nullptr, &render_pass) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create render pass");
    }

    return render_pass;
}

RenderPassScope
ShadowRenderPass::begin(VkCommandBuffer command_buffer, VkFramebuffer framebuffer, uint32_t resolution) const
{
    VkClearValue clear_value = {};
    clear_value.depthStencil = {.depth = 1.0F, .stencil = 0};

    VkRenderPassBeginInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = m_render_pass;
    render_pass_info.framebuffer = framebuffer;
    render_pass_info.renderArea = {.offset = {0, 0}, .extent = {resolution, resolution}};
    render_pass_info.clearValueCount = 1;
    render_pass_info.pClearValues = &clear_value;

    vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    return RenderPassScope(command_buffer);
}
