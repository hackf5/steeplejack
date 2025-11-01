#pragma once

#include "device.h"
#include "descriptor_set_layout.h"
#include "shadow_render_pass.h"
#include "util/no_copy_or_move.h"

#include <vulkan/vulkan.h>

#include <memory>

namespace steeplejack
{
class ShadowPipeline : public NoCopyOrMove
{
  private:
    const Device& m_device;
    const std::unique_ptr<DescriptorSetLayout> m_descriptor_set_layout;
    const VkPipelineLayout m_pipeline_layout;
    const VkPipeline m_pipeline;

    std::unique_ptr<DescriptorSetLayout> create_descriptor_set_layout() const;
    VkPipeline create_pipeline(const ShadowRenderPass& shadow_render_pass,
                                 const std::string& vertex_shader,
                                 const std::string& fragment_shader) const;

  public:
    ShadowPipeline(const Device& device,
                   const ShadowRenderPass& shadow_render_pass,
                   const std::string& vertex_shader,
                   const std::string& fragment_shader);
    ~ShadowPipeline();

};
} // namespace steeplejack