#pragma once

#include "VkBootstrap.h"
#include "window.h"

#include <vma/vk_mem_alloc.h>

namespace steeplejack
{
class Device
{
  private:
    const Window& m_window;

    const vkb::Instance m_instance;
    const VkSurfaceKHR m_surface;
    const vkb::Device m_device;
    const VmaAllocator m_allocator;

    const VkQueue m_graphics_queue;
    const VkQueue m_present_queue;
    const VkQueue m_transfer_queue;

    static vkb::Instance create_instance(bool enable_validation_layers);
    VkSurfaceKHR create_surface();
    vkb::Device create_device();
    VmaAllocator create_allocator();
    VkQueue create_queue(vkb::QueueType queue_type);

  public:
    Device(const Window& window, bool enable_validation_layers);

    ~Device();

    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;
    Device(Device&&) = delete;
    Device& operator=(Device&&) = delete;

    static const uint32_t kMaxFramesInFlight = 2;

    [[nodiscard]] const vkb::Device& vkb() const
    {
        return m_device;
    }

    [[nodiscard]] VkDevice vk() const
    {
        return m_device.device;
    }

    [[nodiscard]] VkPhysicalDeviceProperties properties() const
    {
        return m_device.physical_device.properties;
    }

    [[nodiscard]] VkSampleCountFlagBits msaa_samples() const
    {
        auto counts = m_device.physical_device.properties.limits.framebufferColorSampleCounts &
            m_device.physical_device.properties.limits.framebufferDepthSampleCounts;
        if ((counts & VK_SAMPLE_COUNT_64_BIT) != 0U)
        {
            return VK_SAMPLE_COUNT_64_BIT;
        }
        if ((counts & VK_SAMPLE_COUNT_32_BIT) != 0U)
        {
            return VK_SAMPLE_COUNT_32_BIT;
        }
        if ((counts & VK_SAMPLE_COUNT_16_BIT) != 0U)
        {
            return VK_SAMPLE_COUNT_16_BIT;
        }
        if ((counts & VK_SAMPLE_COUNT_8_BIT) != 0U)
        {
            return VK_SAMPLE_COUNT_8_BIT;
        }
        if ((counts & VK_SAMPLE_COUNT_4_BIT) != 0U)
        {
            return VK_SAMPLE_COUNT_4_BIT;
        }
        if ((counts & VK_SAMPLE_COUNT_2_BIT) != 0U)
        {
            return VK_SAMPLE_COUNT_2_BIT;
        }

        return VK_SAMPLE_COUNT_1_BIT;
    }

    [[nodiscard]] VkInstance instance() const
    {
        return m_instance.instance;
    }

    [[nodiscard]] VkPhysicalDevice physical_device() const
    {
        return m_device.physical_device;
    }

    [[nodiscard]] VkQueue graphics_queue() const
    {
        return m_graphics_queue;
    }
    [[nodiscard]] uint32_t graphics_queue_index() const
    {
        return m_device.get_queue_index(vkb::QueueType::graphics).value();
    }

    [[nodiscard]] VkQueue present_queue() const
    {
        return m_present_queue;
    }
    [[nodiscard]] uint32_t present_queue_index() const
    {
        return m_device.get_queue_index(vkb::QueueType::present).value();
    }

    [[nodiscard]] VkQueue transfer_queue() const
    {
        return m_transfer_queue;
    }
    [[nodiscard]] uint32_t transfer_queue_index() const
    {
        return m_device.get_queue_index(vkb::QueueType::transfer).value();
    }

    void wait_idle() const
    {
        vkDeviceWaitIdle(m_device);
    }

    [[nodiscard]] VmaAllocator allocator() const
    {
        return m_allocator;
    }
};
} // namespace steeplejack
