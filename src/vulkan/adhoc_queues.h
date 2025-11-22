#pragma once

#include <cstdint>
#include <vulkan/vulkan.h>

namespace steeplejack
{
class Device;

enum AdhocQueueFamily : std::uint8_t
{
    Graphics,
    Present,
    Transfer
};

class AdhocQueue
{
  private:
    const Device& m_device;

    const VkQueue m_queue;
    const VkCommandPool m_command_pool;
    const VkCommandBuffer m_command_buffer;

  public:
    AdhocQueue(const Device& device, AdhocQueueFamily family);
    ~AdhocQueue();

    AdhocQueue(const AdhocQueue&) = delete;
    AdhocQueue& operator=(const AdhocQueue&) = delete;
    AdhocQueue(AdhocQueue&&) = delete;
    AdhocQueue& operator=(AdhocQueue&&) = delete;

    [[nodiscard]] VkCommandBuffer begin() const;
    void submit_and_wait() const;
};

class AdhocQueues
{
  private:
    const AdhocQueue m_transfer_queue;
    const AdhocQueue m_graphics_queue;
    const AdhocQueue m_present_queue;

  public:
    explicit AdhocQueues(const Device& device) :
        m_transfer_queue(device, AdhocQueueFamily::Transfer),
        m_graphics_queue(device, AdhocQueueFamily::Graphics),
        m_present_queue(device, AdhocQueueFamily::Present)
    {
    }

    AdhocQueues(const AdhocQueues&) = delete;
    AdhocQueues& operator=(const AdhocQueues&) = delete;
    AdhocQueues(AdhocQueues&&) = delete;
    AdhocQueues& operator=(AdhocQueues&&) = delete;
    ~AdhocQueues() = default;

    [[nodiscard]] const AdhocQueue& transfer() const
    {
        return m_transfer_queue;
    }
    [[nodiscard]] const AdhocQueue& graphics() const
    {
        return m_graphics_queue;
    }
    [[nodiscard]] const AdhocQueue& present() const
    {
        return m_present_queue;
    }
};
} // namespace steeplejack
