#pragma once

#include "adhoc_queue.h"

namespace steeplejack
{
class Device;

class AdhocQueues
{
  private:
    const AdhocQueue m_transfer_queue;
    const AdhocQueue m_graphics_queue;
    const AdhocQueue m_present_queue;

  public:
    explicit AdhocQueues(const Device& device);

    AdhocQueues(const AdhocQueues&) = delete;
    AdhocQueues& operator=(const AdhocQueues&) = delete;
    AdhocQueues(AdhocQueues&&) = delete;
    AdhocQueues& operator=(AdhocQueues&&) = delete;
    ~AdhocQueues() = default;

    [[nodiscard]] const AdhocQueue& transfer() const;
    [[nodiscard]] const AdhocQueue& graphics() const;
    [[nodiscard]] const AdhocQueue& present() const;
};
} // namespace steeplejack
