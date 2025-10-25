#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "util/no_copy_or_move.h"

#include "vulkan/device.h"
#include "vulkan/buffer/uniform_buffer.h"
#include "vulkan/graphics_pipeline.h"
#include "vulkan/texture.h"

#include "primitive.h"

namespace levin
{
class Mesh: NoCopyOrMove
{
private:
    struct UniformBlock
    {
        glm::mat4 model;
    };

    UniformBlock m_uniform_block;
    UniformBuffer m_uniform_buffers;
    std::vector<Primitive> m_primitives;
    Texture *m_texture;

public:
    Mesh(
        const Device &device,
        const std::vector<Primitive> &primitives,
        Texture *texture = nullptr):
        m_uniform_block {},
        m_uniform_buffers(device, sizeof(UniformBlock)),
        m_primitives(primitives),
        m_texture(texture)
    {
    }

    const glm::mat4 &model() const { return m_uniform_block.model; }
    glm::mat4 &model() { return m_uniform_block.model; }

    void set_texture(Texture *texture) { m_texture = texture; }

    void flush(uint32_t frame_index)
    {
        m_uniform_buffers[frame_index].copy_from(m_uniform_block);
    }

    void render(
        VkCommandBuffer command_buffer,
        uint32_t frame_index,
        GraphicsPipeline &pipeline)
    {
        pipeline
            .descriptor_set_layout()
            .write_uniform_buffer(m_uniform_buffers[frame_index].descriptor(), 1);

        if (m_texture)
        {
            pipeline
                .descriptor_set_layout()
                .write_combined_image_sampler(m_texture->descriptor(), 2);
        }

        pipeline.push_descriptor_set(command_buffer);

        for (auto &primitive : m_primitives)
        {
            primitive.render(command_buffer);
        }
    }
};
}