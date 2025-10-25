#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "util/no_copy_or_move.h"
#include "vulkan/device.h"

namespace steeplejack {

class AdhocQueue : NoCopyOrMove {
public:
    enum QueueFamily { graphics, present, transfer };

    AdhocQueue(const Device& device, QueueFamily family);

private:
    const Device& device_;
    const VkQueue queue_;
    const VkCommandPool command_pool_;
    const VkCommandBuffer command_buffer_;

    VkQueue get_queue(QueueFamily family) const;
    uint32_t get_queue_index(QueueFamily family) const;
    VkCommandPool create_command_pool(QueueFamily family);
    VkCommandBuffer create_command_buffer();

public:
    ~AdhocQueue();
    VkCommandBuffer begin() const;
    void submit_and_wait() const;
};

class AdhocQueues : NoCopyOrMove {
private:
    const AdhocQueue transfer_;
    const AdhocQueue graphics_;
    const AdhocQueue present_;

public:
    explicit AdhocQueues(const Device& device)
        : transfer_(device, AdhocQueue::transfer),
          graphics_(device, AdhocQueue::graphics),
          present_(device, AdhocQueue::present) {}

    const AdhocQueue& transfer() const { return transfer_; }
    const AdhocQueue& graphics() const { return graphics_; }
    const AdhocQueue& present() const { return present_; }
};

} // namespace steeplejack

