#pragma once

#include "vulkan/pipeline/base_pipeline.h"

namespace steeplejack
{
class ShadowRenderPass;
}

namespace steeplejack::pipeline
{
class ShadowPipeline final : public BasePipeline
{
  public:
    ShadowPipeline(const Device& device, const ShadowRenderPass& render_pass, const std::string& pipeline_name);
    ~ShadowPipeline() override = default;

    ShadowPipeline(const ShadowPipeline&) = delete;
    ShadowPipeline& operator=(const ShadowPipeline&) = delete;
    ShadowPipeline(ShadowPipeline&&) = delete;
    ShadowPipeline& operator=(ShadowPipeline&&) = delete;
};
} // namespace steeplejack::pipeline
