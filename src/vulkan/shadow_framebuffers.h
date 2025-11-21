#pragma once

#include "device.h"
#include "shadow_map_array.h"
#include "shadow_render_pass.h"

#include <vector>
#include <vulkan/vulkan.h>

namespace steeplejack
{
class ShadowFramebuffers
{
  private:
    const Device& m_device;
    const std::vector<VkFramebuffer> m_framebuffers;

  public:
    ShadowFramebuffers(
        const Device& device, const ShadowMapArray& shadow_map_array, const ShadowRenderPass& shadow_render_pass);
    ~ShadowFramebuffers();

    ShadowFramebuffers(const ShadowFramebuffers&) = delete;
    ShadowFramebuffers& operator=(const ShadowFramebuffers&) = delete;
    ShadowFramebuffers(ShadowFramebuffers&&) = delete;
    ShadowFramebuffers& operator=(ShadowFramebuffers&&) = delete;

    [[nodiscard]] size_t size() const;

    [[nodiscard]] VkFramebuffer at(size_t index) const;
};
} // namespace steeplejack
