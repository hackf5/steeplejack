#pragma once

#include "device.h"
#include "shadow_map_array.h"
#include "shadow_render_pass.h"
#include "util/no_copy_or_move.h"

#include <vector>
#include <vulkan/vulkan.h>

namespace steeplejack
{
class ShadowFramebuffers : public NoCopyOrMove
{
  private:
    const Device& m_device;
    const std::vector<VkFramebuffer> m_framebuffers;

    std::vector<VkFramebuffer> create_framebuffers(const ShadowMapArray& shadow_map_array,
                       const ShadowRenderPass& shadow_render_pass);

  public:
    ShadowFramebuffers(const Device& device,
                       const ShadowMapArray& shadow_map_array,
                       const ShadowRenderPass& shadow_render_pass);
    ~ShadowFramebuffers();

    size_t size() const
    {
        return m_framebuffers.size();
    }

    std::vector<VkFramebuffer>::const_iterator begin() const
    {
        return m_framebuffers.begin();
    }

    std::vector<VkFramebuffer>::const_iterator end() const
    {
        return m_framebuffers.end();
    }
};
} // namespace steeplejack