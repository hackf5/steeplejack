#pragma once

#include "VkBootstrap.h"

#include <vma/vk_mem_alloc.h>

namespace steeplejack
{
class Window;

class Device
{
  private:
    const vkb::Instance m_instance;
    const VkSurfaceKHR m_surface;
    const vkb::Device m_device;
    const VmaAllocator m_allocator;

    const VkQueue m_graphics_queue;
    const VkQueue m_present_queue;
    const VkQueue m_transfer_queue;

  public:
    Device(const Window& window, bool enable_validation_layers);
    ~Device();

    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;
    Device(Device&&) = delete;
    Device& operator=(Device&&) = delete;

    static constexpr uint32_t kMaxFramesInFlight = 2;

    [[nodiscard]] VkDevice vk() const;
    [[nodiscard]] const vkb::Device& vkb() const;

    [[nodiscard]] VkInstance instance() const;
    [[nodiscard]] VkPhysicalDevice physical_device() const;
    [[nodiscard]] VkPhysicalDeviceProperties properties() const;
    [[nodiscard]] VmaAllocator allocator() const;

    [[nodiscard]] VkQueue graphics_queue() const;
    [[nodiscard]] uint32_t graphics_queue_index() const;

    [[nodiscard]] VkQueue present_queue() const;
    [[nodiscard]] uint32_t present_queue_index() const;

    [[nodiscard]] VkQueue transfer_queue() const;
    [[nodiscard]] uint32_t transfer_queue_index() const;

    [[nodiscard]] VkSampleCountFlagBits msaa_samples() const;

    void wait_idle() const;
};
} // namespace steeplejack
