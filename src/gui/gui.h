#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include "vulkan/window.h"
#include "vulkan/device.h"
#include "vulkan/swapchain.h"
#include "vulkan/render_pass.h"
#include "framerate.h"

namespace steeplejack
{
class Gui
{
private:
    const Device &m_device;
    VkDescriptorPool m_descriptor_pool;
    Framerate<> m_framerate;

    VkDescriptorPool create_descriptor_pool();

    static void check_vk_result(VkResult result);

public:
    Gui(
        const Window &window,
        const Device &device,
        const RenderPass &render_pass);
    ~Gui();

    void begin_frame();
    void render(VkCommandBuffer command_buffer) const;
};
} //
