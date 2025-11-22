#define VMA_IMPLEMENTATION
#include "device.h"

#include "spdlog/spdlog.h"
#include "window.h"

#include <array>
#include <stdexcept>
#include <vma/vk_mem_alloc.h>

using namespace steeplejack;

namespace
{
vkb::Instance create_instance(bool enable_validation_layers)
{
    spdlog::info("Creating Vulkan Instance");

    vkb::InstanceBuilder builder;
    auto instance = builder.set_app_name("Steeplejack")
                        .request_validation_layers(enable_validation_layers)
                        .use_default_debug_messenger()
                        .build();
    if (!instance)
    {
        throw std::runtime_error("Failed to create Vulkan Instance: " + instance.error().message());
    }

    return instance.value();
}

VkSurfaceKHR create_surface(const Window& window, VkInstance instance)
{
    return window.create_window_surface(instance);
}

vkb::Device create_device(const vkb::Instance& instance, VkSurfaceKHR surface)
{
    spdlog::info("Selecting Vulkan Physical Device");

    VkPhysicalDeviceFeatures required_features = {};
    required_features.samplerAnisotropy = VK_TRUE;
    required_features.depthClamp = VK_TRUE;

    vkb::PhysicalDeviceSelector selector{instance};
    auto physical_device = selector.set_surface(surface)
                               .set_minimum_version(1, 3)
                               .require_dedicated_transfer_queue()
                               .set_required_features(required_features)
                               .add_required_extension(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME)
                               .select();
    if (!physical_device)
    {
        throw std::runtime_error("Failed to select Vulkan Physical Device: " + physical_device.error().message());
    }

    spdlog::info("Creating Vulkan Device");

    vkb::DeviceBuilder const device_builder{physical_device.value()};
    auto device = device_builder.build();
    if (!device)
    {
        throw std::runtime_error("Failed to create Vulkan Device: " + device.error().message());
    }

    return device.value();
}

VmaAllocator create_allocator(VkInstance instance, VkPhysicalDevice physical_device, VkDevice device)
{
    spdlog::info("Creating Vulkan Memory Allocator");

    VmaAllocatorCreateInfo allocator_info = {};
    allocator_info.physicalDevice = physical_device;
    allocator_info.device = device;
    allocator_info.instance = instance;

    VmaAllocator allocator = nullptr;
    if (vmaCreateAllocator(&allocator_info, &allocator) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Vulkan Memory Allocator");
    }

    return allocator;
}

VkQueue create_queue(const vkb::Device& device, vkb::QueueType queue_type)
{
    auto queue = device.get_queue(queue_type);
    if (!queue)
    {
        throw std::runtime_error("Failed to get queue: " + queue.error().message());
    }

    return queue.value();
}
} // namespace

Device::Device(const Window& window, bool enable_validation_layers) :
    m_window(window),
    m_instance(create_instance(enable_validation_layers)),
    m_surface(create_surface(window, m_instance.instance)),
    m_device(create_device(m_instance, m_surface)),
    m_allocator(create_allocator(m_instance.instance, m_device.physical_device, m_device.device)),
    m_graphics_queue(create_queue(m_device, vkb::QueueType::graphics)),
    m_present_queue(create_queue(m_device, vkb::QueueType::present)),
    m_transfer_queue(create_queue(m_device, vkb::QueueType::transfer))
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

const vkb::Device& Device::vkb() const
{
    return m_device;
}

VkDevice Device::vk() const
{
    return m_device.device;
}

VkPhysicalDeviceProperties Device::properties() const
{
    return m_device.physical_device.properties;
}

VkSampleCountFlagBits Device::msaa_samples() const
{
    VkSampleCountFlags const counts = m_device.physical_device.properties.limits.framebufferColorSampleCounts &
        m_device.physical_device.properties.limits.framebufferDepthSampleCounts;

    static constexpr std::array<VkSampleCountFlagBits, 6> kPreferredCounts = {
        VK_SAMPLE_COUNT_64_BIT,
        VK_SAMPLE_COUNT_32_BIT,
        VK_SAMPLE_COUNT_16_BIT,
        VK_SAMPLE_COUNT_8_BIT,
        VK_SAMPLE_COUNT_4_BIT,
        VK_SAMPLE_COUNT_2_BIT,
    };

    for (auto count : kPreferredCounts)
    {
        if ((counts & count) != 0U)
        {
            return count;
        }
    }

    return VK_SAMPLE_COUNT_1_BIT;
}

VkInstance Device::instance() const
{
    return m_instance.instance;
}

VkPhysicalDevice Device::physical_device() const
{
    return m_device.physical_device;
}

VkQueue Device::graphics_queue() const
{
    return m_graphics_queue;
}

uint32_t Device::graphics_queue_index() const
{
    return m_device.get_queue_index(vkb::QueueType::graphics).value();
}

VkQueue Device::present_queue() const
{
    return m_present_queue;
}

uint32_t Device::present_queue_index() const
{
    return m_device.get_queue_index(vkb::QueueType::present).value();
}

VkQueue Device::transfer_queue() const
{
    return m_transfer_queue;
}

uint32_t Device::transfer_queue_index() const
{
    return m_device.get_queue_index(vkb::QueueType::transfer).value();
}

VmaAllocator Device::allocator() const
{
    return m_allocator;
}

void Device::wait_idle() const
{
    vkDeviceWaitIdle(m_device);
}
