#pragma once

#include "vulkan/pipeline/base_pipeline.h"
#include "vulkan/render_pass.h"

namespace steeplejack
{
class RenderPass;
}

namespace steeplejack::pipeline
{
class GraphicsPipeline final : public BasePipeline
{
  public:
    GraphicsPipeline(const Device& device, const RenderPass& render_pass, const std::string& pipeline_name);
    ~GraphicsPipeline() override = default;

    GraphicsPipeline(const GraphicsPipeline&) = delete;
    GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;
    GraphicsPipeline(GraphicsPipeline&&) = delete;
    GraphicsPipeline& operator=(GraphicsPipeline&&) = delete;
};
} // namespace steeplejack::pipeline
