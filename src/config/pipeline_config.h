#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace steeplejack::config
{
struct DescriptorBinding
{
    std::string name;
    uint32_t binding{};
    std::string type;
    std::vector<std::string> stage_flags;
};

struct VertexBinding
{
    uint32_t binding{};
    uint32_t stride{};
    std::string input_rate;
};

struct VertexAttribute
{
    std::string name;
    uint32_t binding{};
    uint32_t location{};
    std::string format;
    uint32_t offset{};
};

struct PipelineDefinition
{
    std::string name;
    std::string vertex_shader;
    std::string fragment_shader;
    std::vector<VertexBinding> vertex_bindings;
    std::vector<VertexAttribute> vertex_attributes;
    std::vector<DescriptorBinding> descriptor_bindings;
};

class PipelineConfig
{
  private:
    PipelineConfig();

    std::unordered_map<std::string, PipelineDefinition> m_pipelines;

  public:
    static PipelineConfig& instance();

    PipelineDefinition require_pipeline(const std::string& name) const;
};
} // namespace steeplejack::config