#pragma once

#include <memory>
#include <string>
#include <vector>

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

#include "util/no_copy_or_move.h"
#include "buffer/buffer_host.h"
#include "adhoc_queues.h"
#include "device.h"
#include "buffer/buffer.h"
#include "sampler.h"
#include "image.h"
#include "image_view.h"
#include "swapchain.h"
#include "descriptor_set_layout.h"
#include "graphics_pipeline.h"

namespace levin
{
class Texture: public NoCopyOrMove
{
private:
    const Device &m_device;
    const std::string m_name;
    const std::unique_ptr<Image> m_image;
    const ImageView m_image_view;
    VkDescriptorImageInfo m_image_descriptor_info;

    std::unique_ptr<Buffer> create_staging_buffer(const std::string &name, int &width, int &height);
    std::unique_ptr<Image> create_image(const AdhocQueues &adhoc_queues);
    VkDescriptorImageInfo create_image_descriptor_info(const Sampler &sampler);

    void transition_image_layout(
        const AdhocQueues &adhoc_queues,
        VkImageLayout old_layout,
        VkImageLayout new_layout);

    void copy_staging_buffer_to_image(
        const Buffer &staging_buffer,
        const AdhocQueues &adhoc_queues);

public:
    Texture(
        const Device &device,
        const Sampler &sampler,
        const AdhocQueues &adhoc_queues,
        const std::string &name);

    VkDescriptorImageInfo *descriptor() { return &m_image_descriptor_info; }
};
}