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
} // namespace steeplejack
