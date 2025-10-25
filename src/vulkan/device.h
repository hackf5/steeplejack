#pragma once

#include <memory>

#include <vma/vk_mem_alloc.h>
#include <VkBootstrap.h>

#include "util/no_copy_or_move.h"
#include "vulkan/window.h"

namespace steeplejack {

class Device : NoCopyOrMove {
private:
    const Window& window_;

    const vkb::Instance instance_;
    const VkSurfaceKHR surface_;
    const vkb::Device device_;
    const VmaAllocator allocator_;

    const VkQueue graphics_queue_;
    const VkQueue present_queue_;
    const VkQueue transfer_queue_;

    vkb::Instance create_instance(bool enable_validation_layers);
    VkSurfaceKHR create_surface();
    vkb::Device create_device();
    VmaAllocator create_allocator();
    VkQueue create_queue(vkb::QueueType queue_type);

public:
    Device(const Window& window, bool enable_validation_layers);
    ~Device();

    static const uint32_t max_frames_in_flight = 2;

    operator const vkb::Device&() const { return device_; }
    operator VkDevice() const { return device_.device; }

    VkPhysicalDeviceProperties properties() const { return device_.physical_device.properties; }

    VkSampleCountFlagBits msaa_samples() const {
        auto counts = device_.physical_device.properties.limits.framebufferColorSampleCounts &
                      device_.physical_device.properties.limits.framebufferDepthSampleCounts;
        if (counts & VK_SAMPLE_COUNT_64_BIT) return VK_SAMPLE_COUNT_64_BIT;
        if (counts & VK_SAMPLE_COUNT_32_BIT) return VK_SAMPLE_COUNT_32_BIT;
        if (counts & VK_SAMPLE_COUNT_16_BIT) return VK_SAMPLE_COUNT_16_BIT;
        if (counts & VK_SAMPLE_COUNT_8_BIT) return VK_SAMPLE_COUNT_8_BIT;
        if (counts & VK_SAMPLE_COUNT_4_BIT) return VK_SAMPLE_COUNT_4_BIT;
        if (counts & VK_SAMPLE_COUNT_2_BIT) return VK_SAMPLE_COUNT_2_BIT;
        return VK_SAMPLE_COUNT_1_BIT;
    }

    VkInstance instance() const { return instance_.instance; }
    VkPhysicalDevice physical_device() const { return device_.physical_device; }

    VkQueue graphics_queue() const { return graphics_queue_; }
    uint32_t graphics_queue_index() const { return device_.get_queue_index(vkb::QueueType::graphics).value(); }

    VkQueue present_queue() const { return present_queue_; }
    uint32_t present_queue_index() const { return device_.get_queue_index(vkb::QueueType::present).value(); }

    VkQueue transfer_queue() const { return transfer_queue_; }
    uint32_t transfer_queue_index() const { return device_.get_queue_index(vkb::QueueType::transfer).value(); }

    void wait_idle() const { vkDeviceWaitIdle(device_); }

    VmaAllocator allocator() const { return allocator_; }
};

} // namespace steeplejack

