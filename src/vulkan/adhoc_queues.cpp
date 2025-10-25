#include "vulkan/adhoc_queues.h"

#include <stdexcept>
#include "spdlog/spdlog.h"

using namespace steeplejack;

AdhocQueue::AdhocQueue(const Device& device, QueueFamily family)
    : device_(device),
      queue_(get_queue(family)),
      command_pool_(create_command_pool(family)),
      command_buffer_(create_command_buffer()) {}

AdhocQueue::~AdhocQueue() {
    spdlog::info("Destroying AdhocQueue");
    vkFreeCommandBuffers(device_, command_pool_, 1, &command_buffer_);
    vkDestroyCommandPool(device_, command_pool_, nullptr);
}

VkQueue AdhocQueue::get_queue(QueueFamily family) const {
    switch (family) {
        case graphics:
            return device_.graphics_queue();
        case present:
            return device_.present_queue();
        case transfer:
            return device_.transfer_queue();
        default:
            throw std::runtime_error("Invalid queue family");
    }
}

uint32_t AdhocQueue::get_queue_index(QueueFamily family) const {
    switch (family) {
        case graphics:
            return device_.graphics_queue_index();
        case present:
            return device_.present_queue_index();
        case transfer:
            return device_.transfer_queue_index();
        default:
            throw std::runtime_error("Invalid queue family");
    }
}

VkCommandPool AdhocQueue::create_command_pool(QueueFamily family) {
    VkCommandPoolCreateInfo pool{};
    pool.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool.queueFamilyIndex = get_queue_index(family);
    pool.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    VkCommandPool cp{};
    if (vkCreateCommandPool(device_, &pool, nullptr, &cp) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool");
    }
    return cp;
}

VkCommandBuffer AdhocQueue::create_command_buffer() {
    VkCommandBufferAllocateInfo ai{};
    ai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    ai.commandPool = command_pool_;
    ai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    ai.commandBufferCount = 1;
    VkCommandBuffer cb{};
    if (vkAllocateCommandBuffers(device_, &ai, &cb) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffers");
    }
    return cb;
}

VkCommandBuffer AdhocQueue::begin() const {
    if (vkResetCommandBuffer(command_buffer_, 0) != VK_SUCCESS) {
        throw std::runtime_error("Failed to reset command buffer");
    }
    VkCommandBufferBeginInfo bi{};
    bi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    if (vkBeginCommandBuffer(command_buffer_, &bi) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording command buffer");
    }
    return command_buffer_;
}

void AdhocQueue::submit_and_wait() const {
    auto cb = command_buffer_;
    if (vkEndCommandBuffer(cb) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer");
    }
    VkSubmitInfo si{};
    si.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    si.commandBufferCount = 1;
    si.pCommandBuffers = &cb;
    if (vkQueueSubmit(queue_, 1, &si, VK_NULL_HANDLE) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit command buffer");
    }
    if (vkQueueWaitIdle(queue_) != VK_SUCCESS) {
        throw std::runtime_error("Failed to wait queue idle");
    }
}

