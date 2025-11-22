#include "adhoc_queues.h"

#include "device.h"

using namespace steeplejack;

AdhocQueues::AdhocQueues(const Device& device) :
    m_transfer_queue(device, AdhocQueueFamily::Transfer),
    m_graphics_queue(device, AdhocQueueFamily::Graphics),
    m_present_queue(device, AdhocQueueFamily::Present)
{
}

const AdhocQueue& AdhocQueues::transfer() const
{
    return m_transfer_queue;
}

const AdhocQueue& AdhocQueues::graphics() const
{
    return m_graphics_queue;
}

const AdhocQueue& AdhocQueues::present() const
{
    return m_present_queue;
}
