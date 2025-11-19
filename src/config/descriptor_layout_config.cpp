#include "config/descriptor_layout_config.h"

#include "spdlog/spdlog.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fmt/format.h>
#include <stdexcept>
#include <string>
#include <toml++/toml.h>
#include <unordered_set>

namespace
{
using namespace steeplejack::config;

constexpr std::string_view kNameKey = "name";
constexpr std::string_view kBindingKey = "binding";
constexpr std::string_view kBindingsKey = "bindings";
constexpr std::string_view kTypeKey = "type";
constexpr std::string_view kCountKey = "count";
constexpr std::string_view kStageFlagsKey = "stage_flags";

std::string to_lower(std::string_view value)
{
    std::string result(value);
    std::ranges::transform(result, result.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return result;
}

VkDescriptorType descriptor_type_from_string(std::string_view type_name)
{
    static const std::unordered_map<std::string_view, VkDescriptorType> descriptor_type_map = {
        {"combined_image_sampler", VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER},
        {"uniform_buffer", VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER},
        {"uniform_buffer_dynamic", VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC},
        {"storage_buffer", VK_DESCRIPTOR_TYPE_STORAGE_BUFFER},
        {"storage_buffer_dynamic", VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC},
        {"storage_image", VK_DESCRIPTOR_TYPE_STORAGE_IMAGE},
        {"sampler", VK_DESCRIPTOR_TYPE_SAMPLER},
        {"sampled_image", VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE},
    };

    auto lower = to_lower(type_name);
    auto it = descriptor_type_map.find(lower);
    if (it == descriptor_type_map.end())
    {
        throw std::runtime_error(fmt::format("Unsupported descriptor type '{}'", type_name));
    }

    return it->second;
}

VkShaderStageFlags stage_flag_from_string(std::string_view stage_name)
{
    static const std::unordered_map<std::string_view, VkShaderStageFlagBits> stage_map = {
        {"vertex", VK_SHADER_STAGE_VERTEX_BIT},
        {"fragment", VK_SHADER_STAGE_FRAGMENT_BIT},
        {"compute", VK_SHADER_STAGE_COMPUTE_BIT},
        {"geometry", VK_SHADER_STAGE_GEOMETRY_BIT},
        {"tessellation_control", VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT},
        {"tessellation_evaluation", VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT},
        {"all_graphics", VK_SHADER_STAGE_ALL_GRAPHICS},
        {"all", VK_SHADER_STAGE_ALL},
    };

    auto lower = to_lower(stage_name);
    auto it = stage_map.find(lower);
    if (it == stage_map.end())
    {
        throw std::runtime_error(fmt::format("Unsupported stage flag '{}'", stage_name));
    }

    return it->second;
}

VkShaderStageFlags parse_stage_flags(
    const toml::array& stage_nodes,
    const std::filesystem::path& source_file,
    std::string_view layout_name,
    std::string_view binding_name)
{
    if (stage_nodes.empty())
    {
        throw std::runtime_error(
            fmt::format(
                "Descriptor layout '{}' binding '{}' in '{}' must specify at least one stage flag",
                layout_name,
                binding_name,
                source_file.string()));
    }

    VkShaderStageFlags flags = 0;
    for (const auto& node : stage_nodes)
    {
        auto value = node.value<std::string>();
        if (!value)
        {
            throw std::runtime_error(
                fmt::format(
                    "Stage flag entry for binding '{}' in layout '{}' must be a string ({})",
                    binding_name,
                    layout_name,
                    source_file.string()));
        }

        flags |= stage_flag_from_string(*value);
    }

    return flags;
}

DescriptorBindingDefinition
parse_binding(const toml::table& binding_table, const std::filesystem::path& source_file, std::string_view layout_name)
{
    auto name_value = binding_table[kNameKey].value<std::string>();
    if (!name_value || name_value->empty())
    {
        throw std::runtime_error(
            fmt::format(
                "Binding entry in '{}' for layout '{}' is missing a non-empty 'name'",
                source_file.string(),
                layout_name));
    }

    auto binding_index = binding_table[kBindingKey].value<int64_t>();
    if (!binding_index || *binding_index < 0)
    {
        throw std::runtime_error(
            fmt::format(
                "Binding '{}' in layout '{}' specifies an invalid 'binding' index in '{}'",
                *name_value,
                layout_name,
                source_file.string()));
    }

    auto type_value = binding_table[kTypeKey].value<std::string>();
    if (!type_value)
    {
        throw std::runtime_error(
            fmt::format(
                "Binding '{}' in layout '{}' must specify a descriptor 'type' in '{}'",
                *name_value,
                layout_name,
                source_file.string()));
    }

    DescriptorBindingDefinition result;
    result.name = *name_value;
    result.binding = static_cast<uint32_t>(*binding_index);
    result.type = descriptor_type_from_string(*type_value);

    auto count_value = binding_table[kCountKey].value<uint32_t>();
    result.count = count_value.value_or(1);
    if (result.count == 0)
    {
        throw std::runtime_error(
            fmt::format(
                "Binding '{}' in layout '{}' has invalid 'count' of 0 in '{}'",
                result.name,
                layout_name,
                source_file.string()));
    }

    const auto* stage_flags_node = binding_table[kStageFlagsKey].as_array();
    if (stage_flags_node == nullptr)
    {
        throw std::runtime_error(
            fmt::format(
                "Binding '{}' in layout '{}' must specify an array of '{}' entries in '{}'",
                result.name,
                layout_name,
                kStageFlagsKey,
                source_file.string()));
    }

    result.stage_flags = parse_stage_flags(*stage_flags_node, source_file, layout_name, result.name);
    return result;
}

DescriptorLayoutDefinition parse_layout_file(const std::filesystem::path& file_path)
{
    toml::table table;
    try
    {
        table = toml::parse_file(file_path.string());
    }
    catch (const toml::parse_error& err)
    {
        throw std::runtime_error(
            fmt::format("Failed to parse descriptor layout file '{}': {}", file_path.string(), err.description()));
    }

    auto layout_name = table[kNameKey].value<std::string>();
    if (!layout_name || layout_name->empty())
    {
        throw std::runtime_error(
            fmt::format("Descriptor layout in '{}' must specify a non-empty 'name'", file_path.string()));
    }

    const auto* bindings_array = table[kBindingsKey].as_array();
    if (bindings_array == nullptr)
    {
        throw std::runtime_error(
            fmt::format(
                "Descriptor layout '{}' in '{}' must contain a '{}' array",
                *layout_name,
                file_path.string(),
                kBindingsKey));
    }

    DescriptorLayoutDefinition definition;
    definition.name = *layout_name;

    std::unordered_set<uint32_t> seen_bindings;
    std::unordered_set<std::string> seen_names;

    for (const auto& binding_node : *bindings_array)
    {
        const auto* binding_table = binding_node.as_table();
        if (binding_table == nullptr)
        {
            throw std::runtime_error(
                fmt::format(
                    "Descriptor layout '{}' in '{}' contains a non-table binding entry",
                    definition.name,
                    file_path.string()));
        }

        auto binding_definition = parse_binding(*binding_table, file_path, definition.name);
        if (!seen_bindings.insert(binding_definition.binding).second)
        {
            throw std::runtime_error(
                fmt::format(
                    "Descriptor layout '{}' in '{}' has duplicate binding index {}",
                    definition.name,
                    file_path.string(),
                    binding_definition.binding));
        }
        if (!seen_names.insert(binding_definition.name).second)
        {
            throw std::runtime_error(
                fmt::format(
                    "Descriptor layout '{}' in '{}' has duplicate binding name '{}'",
                    definition.name,
                    file_path.string(),
                    binding_definition.name));
        }

        definition.bindings.push_back(std::move(binding_definition));
    }

    if (definition.bindings.empty())
    {
        throw std::runtime_error(
            fmt::format(
                "Descriptor layout '{}' defined in '{}' does not contain any bindings",
                definition.name,
                file_path.string()));
    }

    std::ranges::sort(
        definition.bindings,
        [](const DescriptorBindingDefinition& lhs, const DescriptorBindingDefinition& rhs)
        { return lhs.binding < rhs.binding; });

    spdlog::debug("Loaded descriptor layout '{}' from '{}'", definition.name, file_path.string());
    return definition;
}
} // namespace

namespace steeplejack::config
{
const DescriptorLayoutConfig& DescriptorLayoutConfig::instance()
{
    static DescriptorLayoutConfig const instance;
    return instance;
}

DescriptorLayoutConfig::DescriptorLayoutConfig()
{
    const auto directory = std::filesystem::path("config") / "descriptor_layouts";
    if (!std::filesystem::exists(directory))
    {
        throw std::runtime_error(fmt::format("Descriptor layout config path '{}' does not exist", directory.string()));
    }

    if (!std::filesystem::is_directory(directory))
    {
        throw std::runtime_error(
            fmt::format("Descriptor layout config path '{}' is not a directory", directory.string()));
    }

    for (const auto& entry : std::filesystem::directory_iterator(directory))
    {
        if (!entry.is_regular_file() || entry.path().extension() != ".toml")
        {
            continue;
        }

        auto layout_definition = parse_layout_file(entry.path());
        auto [it, inserted] = m_layouts.emplace(layout_definition.name, std::move(layout_definition));
        if (!inserted)
        {
            throw std::runtime_error(
                fmt::format(
                    "Descriptor layout '{}' defined more than once (second occurrence '{}')",
                    it->first,
                    entry.path().string()));
        }
    }

    if (m_layouts.empty())
    {
        spdlog::warn("No descriptor layouts were loaded from '{}'", directory.string());
    }
}

const DescriptorLayoutDefinition& DescriptorLayoutConfig::require_layout(std::string_view name) const
{
    auto it = m_layouts.find(std::string(name));
    if (it == m_layouts.end())
    {
        throw std::runtime_error(fmt::format("Descriptor layout '{}' was not found in the loaded configuration", name));
    }

    return it->second;
}

const DescriptorBindingDefinition& DescriptorLayoutDefinition::require_binding(std::string_view binding_name) const
{
    auto it = std::ranges::find_if(
        bindings,
        [&](const DescriptorBindingDefinition& binding) { return binding.name == binding_name; });
    if (it == bindings.end())
    {
        throw std::runtime_error(fmt::format("Binding '{}' not found in layout '{}'", binding_name, name));
    }
    return *it;
}
} // namespace steeplejack::config
