#pragma once

#include <memory>

#include <vma/vk_mem_alloc.h>

#include "VkBootstrap.h"

#include "util/no_copy_or_move.h"
#include "window.h"

namespace steeplejack
{
class Device: NoCopyOrMove
{
private:
    const Window &m_window;

    const vkb::Instance m_instance;
    const VkSurfaceKHR m_surface;
    const vkb::Device m_device;
    const VmaAllocator m_allocator;

    const VkQueue m_graphics_queue;
    const VkQueue m_present_queue;
    const VkQueue m_transfer_queue;

    vkb::Instance create_instance(bool enable_validation_layers);
    VkSurfaceKHR create_surface();
    vkb::Device create_device();
    VmaAllocator create_allocator();
    VkQueue create_queue(vkb::QueueType queue_type);

public:
    Device(
        const Window &window,
        bool enable_validation_layers);

    ~Device();

    static const uint32_t max_frames_in_flight = 2;

    operator const vkb::Device &() const { return m_device; }

    operator VkDevice() const { return m_device.device; }

    VkPhysicalDeviceProperties  properties() const
    {
        return m_device.physical_device.properties;
    }

    VkSampleCountFlagBits msaa_samples() const
    {
        auto counts = m_device.physical_device.properties.limits.framebufferColorSampleCounts
            & m_device.physical_device.properties.limits.framebufferDepthSampleCounts;
        if (counts & VK_SAMPLE_COUNT_64_BIT) return VK_SAMPLE_COUNT_64_BIT;
        if (counts & VK_SAMPLE_COUNT_32_BIT) return VK_SAMPLE_COUNT_32_BIT;
        if (counts & VK_SAMPLE_COUNT_16_BIT) return VK_SAMPLE_COUNT_16_BIT;
        if (counts & VK_SAMPLE_COUNT_8_BIT) return VK_SAMPLE_COUNT_8_BIT;
        if (counts & VK_SAMPLE_COUNT_4_BIT) return VK_SAMPLE_COUNT_4_BIT;
        if (counts & VK_SAMPLE_COUNT_2_BIT) return VK_SAMPLE_COUNT_2_BIT;

        return VK_SAMPLE_COUNT_1_BIT;
    }

    VkInstance instance() const { return m_instance.instance; }

    VkPhysicalDevice physical_device() const { return m_device.physical_device; }

    VkQueue graphics_queue() const { return m_graphics_queue; }
    uint32_t graphics_queue_index() const { return m_device.get_queue_index(vkb::QueueType::graphics).value(); }

    VkQueue present_queue() const { return m_present_queue; }
    uint32_t present_queue_index() const { return m_device.get_queue_index(vkb::QueueType::present).value(); }

    VkQueue transfer_queue() const { return m_transfer_queue; }
    uint32_t transfer_queue_index() const { return m_device.get_queue_index(vkb::QueueType::transfer).value(); }

    void wait_idle() const { vkDeviceWaitIdle(m_device); }

    VmaAllocator allocator() const { return m_allocator; }
};
}