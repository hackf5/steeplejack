#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

#include "vulkan/device.h"

#include "spdlog/spdlog.h"

using namespace steeplejack;

Device::Device(const Window& window, bool enable_validation_layers)
    : window_(window),
      instance_(create_instance(enable_validation_layers)),
      surface_(create_surface()),
      device_(create_device()),
      allocator_(create_allocator()),
      graphics_queue_(create_queue(vkb::QueueType::graphics)),
      present_queue_(create_queue(vkb::QueueType::present)),
      transfer_queue_(create_queue(vkb::QueueType::transfer)) {}

Device::~Device() {
    spdlog::info("Destroying Vulkan Device/Instance/Allocator");
    vmaDestroyAllocator(allocator_);
    vkb::destroy_device(device_);
    vkb::destroy_surface(instance_.instance, surface_);
    vkb::destroy_instance(instance_);
}

vkb::Instance Device::create_instance(bool enable_validation_layers) {
    spdlog::info("Creating Vulkan Instance");
    vkb::InstanceBuilder builder;
    auto inst_ret = builder.set_app_name("Steeplejack")
                        .request_validation_layers(enable_validation_layers)
                        .use_default_debug_messenger()
                        .build();
    if (!inst_ret) {
        throw std::runtime_error("Failed to create Vulkan Instance: " + inst_ret.error().message());
    }
    return inst_ret.value();
}

VkSurfaceKHR Device::create_surface() { return window_.create_window_surface(instance_.instance); }

vkb::Device Device::create_device() {
    spdlog::info("Selecting Physical Device and Creating Logical Device");

    VkPhysicalDeviceFeatures required_features{};
    required_features.samplerAnisotropy = VK_TRUE;

    vkb::PhysicalDeviceSelector selector{instance_};
    auto phys_ret = selector.set_surface(surface_)
                         .set_minimum_version(1, 3)
                         .require_dedicated_transfer_queue()
                         .set_required_features(required_features)
                         .add_required_extension(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME)
                         .select();
    if (!phys_ret) {
        throw std::runtime_error("Failed to select Vulkan Physical Device: " + phys_ret.error().message());
    }

    vkb::DeviceBuilder device_builder{phys_ret.value()};
    auto dev_ret = device_builder.build();
    if (!dev_ret) {
        throw std::runtime_error("Failed to create Vulkan Device: " + dev_ret.error().message());
    }
    return dev_ret.value();
}

VmaAllocator Device::create_allocator() {
    spdlog::info("Creating VMA Allocator");
    VmaAllocatorCreateInfo allocator_info{};
    allocator_info.physicalDevice = device_.physical_device;
    allocator_info.device = device_.device;
    allocator_info.instance = instance_.instance;

    VmaAllocator allocator{};
    if (vmaCreateAllocator(&allocator_info, &allocator) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create VMA allocator");
    }
    return allocator;
}

VkQueue Device::create_queue(vkb::QueueType queue_type) {
    auto queue_ret = device_.get_queue(queue_type);
    if (!queue_ret) {
        throw std::runtime_error("Failed to get queue: " + queue_ret.error().message());
    }
    return queue_ret.value();
}

