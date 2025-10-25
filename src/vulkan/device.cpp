#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

#include "device.h"

#include "spdlog/spdlog.h"

using namespace levin;

Device::Device(
    const Window &window,
    bool enable_validation_layers)
    : m_window(window),
    m_instance(create_instance(enable_validation_layers)),
    m_surface(create_surface()),
    m_device(create_device()),
    m_allocator(create_allocator()),
    m_graphics_queue(create_queue(vkb::QueueType::graphics)),
    m_present_queue(create_queue(vkb::QueueType::present)),
    m_transfer_queue(create_queue(vkb::QueueType::transfer))
{
}

Device::~Device()
{
    spdlog::info("Destroying Vulkan Engine Components");

    vmaDestroyAllocator(m_allocator);
    vkb::destroy_device(m_device);
    vkb::destroy_surface(m_instance.instance, m_surface);
    vkb::destroy_instance(m_instance);
}

vkb::Instance Device::create_instance(bool enable_validation_layers)
{
    spdlog::info("Creating Vulkan Instance");

    vkb::InstanceBuilder builder;
    auto inst_ret = builder.set_app_name("Levin")
        .request_validation_layers(enable_validation_layers)
        .use_default_debug_messenger()
        .build();
    if (!inst_ret)
    {
        throw std::runtime_error("Failed to create Vulkan Instance: " + inst_ret.error().message());
    }

    return inst_ret.value();
}

VkSurfaceKHR Device::create_surface()
{
    return m_window.create_window_surface(m_instance.instance);
}

vkb::Device Device::create_device()
{
    spdlog::info("Selecting Vulkan Physical Device");

    VkPhysicalDeviceFeatures required_features = {};
    required_features.samplerAnisotropy = VK_TRUE;

    vkb::PhysicalDeviceSelector selector { m_instance };
    auto phys_ret = selector.set_surface(m_surface)
        .set_minimum_version(1, 3)
        .require_dedicated_transfer_queue()
        .set_required_features(required_features)
        .add_required_extension(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME)
        .select();
    if (!phys_ret)
    {
        throw std::runtime_error("Failed to select Vulkan Physical Device: " + phys_ret.error().message());
    }

    spdlog::info("Creating Vulkan Device");

    vkb::DeviceBuilder device_builder { phys_ret.value() };
    auto dev_ret = device_builder.build();
    if (!dev_ret)
    {
        throw std::runtime_error("Failed to create Vulkan Device: " + dev_ret.error().message());
    }

    return dev_ret.value();
}

VmaAllocator Device::create_allocator()
{
    spdlog::info("Creating Vulkan Memory Allocator");

    VmaAllocatorCreateInfo allocator_info = {};
    allocator_info.physicalDevice = m_device.physical_device;
    allocator_info.device = m_device.device;
    allocator_info.instance = m_instance.instance;

    VmaAllocator allocator;
    if (vmaCreateAllocator(&allocator_info, &allocator) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Vulkan Memory Allocator");
    }

    return allocator;
}

VkQueue Device::create_queue(vkb::QueueType queue_type)
{
    auto queue_ret = m_device.get_queue(queue_type);
    if (!queue_ret)
    {
        throw std::runtime_error("Failed to get queue: " + queue_ret.error().message());
    }

    return queue_ret.value();
}