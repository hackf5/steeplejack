#include "vulkan/graphics_queue.h"

#include <limits>
#include <stdexcept>

#include "spdlog/spdlog.h"

using namespace steeplejack;

GraphicsQueue::GraphicsQueue(const Device& device)
    : device_(device),
      graphics_queue_(device.graphics_queue()),
      command_pool_(create_command_pool()),
      command_buffers_(create_command_buffers()),
      image_available_(create_semaphores()),
      render_finished_(create_semaphores()),
      in_flight_fences_(create_fences()) {}

GraphicsQueue::~GraphicsQueue() {
    spdlog::info("Destroying GraphicsQueue");
    for (auto f : in_flight_fences_) vkDestroyFence(device_, f, nullptr);
    for (auto s : render_finished_) vkDestroySemaphore(device_, s, nullptr);
    for (auto s : image_available_) vkDestroySemaphore(device_, s, nullptr);
    vkDestroyCommandPool(device_, command_pool_, nullptr);
}

VkCommandPool GraphicsQueue::create_command_pool() {
    spdlog::info("Creating Graphics Command Pool");
    VkCommandPoolCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.queueFamilyIndex = device_.graphics_queue_index();
    info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    VkCommandPool pool{};
    if (vkCreateCommandPool(device_, &info, nullptr, &pool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool");
    }
    return pool;
}

std::vector<VkCommandBuffer> GraphicsQueue::create_command_buffers() {
    spdlog::info("Allocating Command Buffers");
    VkCommandBufferAllocateInfo ai{};
    ai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    ai.commandPool = command_pool_;
    ai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    ai.commandBufferCount = Device::max_frames_in_flight;
    std::vector<VkCommandBuffer> buffers(ai.commandBufferCount);
    if (vkAllocateCommandBuffers(device_, &ai, buffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffers");
    }
    return buffers;
}

std::vector<VkSemaphore> GraphicsQueue::create_semaphores() {
    spdlog::info("Creating Semaphores");
    std::vector<VkSemaphore> sems(Device::max_frames_in_flight);
    for (auto& s : sems) {
        VkSemaphoreCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        if (vkCreateSemaphore(device_, &ci, nullptr, &s) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create semaphore");
        }
    }
    return sems;
}

std::vector<VkFence> GraphicsQueue::create_fences() {
    spdlog::info("Creating Fences");
    std::vector<VkFence> fences(Device::max_frames_in_flight);
    for (auto& f : fences) {
        VkFenceCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        ci.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        if (vkCreateFence(device_, &ci, nullptr, &f) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create fence");
        }
    }
    return fences;
}

VkFramebuffer GraphicsQueue::prepare_framebuffer(uint32_t current_frame,
                                                 const Swapchain& swapchain,
                                                 const Framebuffers& framebuffers) {
    assert(swapchain_ == VK_NULL_HANDLE);
    current_frame_ = current_frame;
    swapchain_ = swapchain;

    vkWaitForFences(device_, 1, &in_flight_fences_[current_frame_], VK_TRUE,
                    std::numeric_limits<uint64_t>::max());
    vkResetFences(device_, 1, &in_flight_fences_[current_frame_]);

    VkResult res = vkAcquireNextImageKHR(device_, swapchain, std::numeric_limits<uint64_t>::max(),
                                         image_available_[current_frame_], VK_NULL_HANDLE, &image_index_);
    if (res == VK_ERROR_OUT_OF_DATE_KHR) {
        return nullptr;
    }
    if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swapchain image");
    }
    return framebuffers.get(image_index_);
}

VkCommandBuffer GraphicsQueue::begin_command() const {
    assert(swapchain_ != VK_NULL_HANDLE);
    if (vkResetCommandBuffer(command_buffers_[current_frame_], 0) != VK_SUCCESS) {
        throw std::runtime_error("Failed to reset command buffer");
    }
    VkCommandBufferBeginInfo bi{};
    bi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    if (vkBeginCommandBuffer(command_buffers_[current_frame_], &bi) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin command buffer");
    }
    return command_buffers_[current_frame_];
}

void GraphicsQueue::submit_command() const {
    assert(swapchain_ != VK_NULL_HANDLE);
    if (vkEndCommandBuffer(command_buffers_[current_frame_]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to end command buffer");
    }

    VkSubmitInfo si{};
    si.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    si.commandBufferCount = 1;
    si.pCommandBuffers = &command_buffers_[current_frame_];

    VkSemaphore wait_semaphores[] = {image_available_[current_frame_]};
    VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    si.waitSemaphoreCount = 1;
    si.pWaitSemaphores = wait_semaphores;
    si.pWaitDstStageMask = wait_stages;

    VkSemaphore signal_semaphores[] = {render_finished_[current_frame_]};
    si.signalSemaphoreCount = 1;
    si.pSignalSemaphores = signal_semaphores;

    vkResetFences(device_, 1, &in_flight_fences_[current_frame_]);
    if (vkQueueSubmit(graphics_queue_, 1, &si, in_flight_fences_[current_frame_]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit draw command buffer");
    }
}

bool GraphicsQueue::present_framebuffer() {
    assert(swapchain_ != VK_NULL_HANDLE);
    auto sc = swapchain_;
    swapchain_ = VK_NULL_HANDLE;

    VkPresentInfoKHR pi{};
    pi.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    VkSemaphore wait_sems[] = {render_finished_[current_frame_]};
    pi.waitSemaphoreCount = 1;
    pi.pWaitSemaphores = wait_sems;
    VkSwapchainKHR swapchains[] = {sc};
    pi.swapchainCount = 1;
    pi.pSwapchains = swapchains;
    pi.pImageIndices = &image_index_;

    VkResult res = vkQueuePresentKHR(graphics_queue_, &pi);
    if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR) {
        return false;
    } else if (res != VK_SUCCESS) {
        throw std::runtime_error("Failed to present swapchain image");
    }
    return true;
}

