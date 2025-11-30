#include "config/pipeline_config.h"

#include <toml++/toml.h>
#include <filesystem>
#include <stdexcept>

using namespace steeplejack::config;

namespace
{
std::vector<std::string> to_strings(const toml::array* arr)
{
    std::vector<std::string> out;
    if (arr == nullptr)
    {
        return out;
    }
    out.reserve(arr->size());
    for (const auto& v : *arr)
    {
        if (auto s = v.value<std::string>())
        {
            out.emplace_back(*s);
        }
    }
    return out;
}

std::vector<DescriptorBinding> parse_descriptor_bindings(const toml::table& tbl)
{
    std::vector<DescriptorBinding> bindings;
    if (const auto *arr = tbl.get_as<toml::array>("bindings"))
    {
        bindings.reserve(arr->size());
        for (const auto& item : *arr)
        {
            if (!item.is_table())
            {
                continue;
            }
            const auto& t = *item.as_table();
            DescriptorBinding b{};
            b.name = t["name"].value_or("");
            b.binding = t["binding"].value_or(0U);
            b.type = t["type"].value_or("");
            b.stage_flags = to_strings(t.get_as<toml::array>("stage_flags"));
            bindings.emplace_back(std::move(b));
        }
    }
    return bindings;
}

std::vector<VertexBinding> parse_vertex_bindings(const toml::table& tbl)
{
    std::vector<VertexBinding> bindings;
    if (const auto *arr = tbl.get_as<toml::array>("bindings"))
    {
        bindings.reserve(arr->size());
        for (const auto& item : *arr)
        {
            if (!item.is_table())
            {
                continue;
            }
            const auto& t = *item.as_table();
            VertexBinding b{};
            b.binding = t["binding"].value_or(0U);
            b.stride = t["stride"].value_or(0U);
            b.input_rate = t["input_rate"].value_or("vertex");
            bindings.emplace_back(std::move(b));
        }
    }
    return bindings;
}

std::vector<VertexAttribute> parse_vertex_attributes(const toml::table& tbl)
{
    std::vector<VertexAttribute> attributes;
    if (const auto *arr = tbl.get_as<toml::array>("attributes"))
    {
        attributes.reserve(arr->size());
        for (const auto& item : *arr)
        {
            if (!item.is_table())
            {
                continue;
            }
            const auto& t = *item.as_table();
            VertexAttribute a{};
            a.name = t["name"].value_or("");
            a.binding = t["binding"].value_or(0U);
            a.location = t["location"].value_or(0U);
            a.format = t["format"].value_or("");
            a.offset = t["offset"].value_or(0U);
            attributes.emplace_back(std::move(a));
        }
    }
    return attributes;
}

PipelineDefinition parse_pipeline_file(const std::string& path)
{
    toml::table tbl = toml::parse_file(path);

    PipelineDefinition def{};
    def.name = tbl["name"].value_or("");

    if (auto *shaders = tbl.get_as<toml::table>("shaders"))
    {
        def.vertex_shader = shaders->operator[]("vertex").value_or("");
        def.fragment_shader = shaders->operator[]("fragment").value_or("");
    }

    if (auto *vi = tbl.get_as<toml::table>("vertex_input"))
    {
        def.vertex_bindings = parse_vertex_bindings(*vi);
        def.vertex_attributes = parse_vertex_attributes(*vi);
    }

    if (auto *dl = tbl.get_as<toml::table>("descriptor_layout"))
    {
        def.descriptor_bindings = parse_descriptor_bindings(*dl);
    }

    return def;
}
} // namespace

PipelineConfig::PipelineConfig()
{
    const std::string base_path = "config/pipelines";
    for (const auto& entry : std::filesystem::directory_iterator(base_path))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }

        const auto& path = entry.path();
        if (path.extension() != ".toml")
        {
            continue;
        }

        auto def = parse_pipeline_file(path.string());
        if (!def.name.empty())
        {
            m_pipelines.emplace(def.name, std::move(def));
        }
    }
}

PipelineConfig& PipelineConfig::instance()
{
    static PipelineConfig inst;
    return inst;
}

PipelineDefinition PipelineConfig::require_pipeline(const std::string& name) const
{
    auto it = m_pipelines.find(name);
    if (it == m_pipelines.end())
    {
        throw std::runtime_error("Pipeline config not found: " + name);
    }
    return it->second;
}
