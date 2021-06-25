// clang-format off
#include <shadow/Basic/BasicCommandBuffers.hpp>
#include <stddef.h>                     // for size_t
#include <stdint.h>                     // for uint32_t
#include <common/VulkanHeader.hpp>         // for VkCommandBuffer, VkCommandBuf...
#include <stdexcept>                    // for runtime_error
#include <common/CommandPool.hpp>       // for CommandPool, vkl
#include <common/DescriptorSets.hpp>    // for DescriptorSets
#include <common/Device.hpp>            // for Device
#include <common/GraphicsPipeline.hpp>  // for GraphicsPipeline
#include <common/RenderPass.hpp>        // for RenderPass
#include <common/SwapChain.hpp>         // for SwapChain
#include <common/buffer/Buffer.hpp>     // for Buffer
#include <common/struct/Vertex.hpp>     // for Vertex
// clang-format on

using namespace vkl;

void BasicCommandBuffers::createCommandBuffers() {
  m_commandBuffers.resize(m_renderPass.size());

  const VkCommandBufferAllocateInfo allocInfo = {
      .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .commandPool        = m_commandPool.handle(),
      .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size()),
  };

  if (vkAllocateCommandBuffers(m_device.logical(), &allocInfo, m_commandBuffers.data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }

  for (size_t i = 0; i < m_commandBuffers.size(); i++) {
    const VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    };

    if (vkBeginCommandBuffer(m_commandBuffers.at(i), &beginInfo) != VK_SUCCESS) {
      throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkClearValue clearValues[2];
    clearValues[0].color        = {{0.0f, 0.0f, 0.2f, 0.0f}};
    clearValues[1].depthStencil = {1.0f, 0};

    const VkRenderPassBeginInfo renderPassBeginInfo = {
          .sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
          .renderPass        = m_renderPass.handle(),
          .framebuffer       = m_renderPass.frameBuffer(i),
          .renderArea = {
              .offset = {0, 0},
              .extent = m_swapChain.extent(),
          },
          .clearValueCount   = 2,
          .pClearValues      = clearValues,
      };

    vkCmdBeginRenderPass(m_commandBuffers.at(i), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    // {
    //   // Set depth bias (aka "Polygon offset")
    //   // Required to avoid shadow mapping artifacts
    //   vkCmdSetDepthBias(m_commandBuffers.at(i), 1.25f, 0.0f, 1.75f);

    //   vkCmdBindPipeline(m_commandBuffers.at(i), VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline.depthPipeline());
    //   vkCmdBindDescriptorSets(m_commandBuffers.at(i), VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline.layout(),
    //   0, 1, &(m_descriptorSets.depthDescriptor(i)), 0, nullptr);

    //   const VkBuffer vertexBuffers[] = {m_vertexBuffer.buffer()};
    //   const VkDeviceSize offsets[]   = {0};
    //   vkCmdBindVertexBuffers(m_commandBuffers.at(i), 0, 1, vertexBuffers, offsets);
    //   vkCmdDraw(m_commandBuffers.at(i), static_cast<uint32_t>(m_vertexBuffer.data().size()), 1, 0, 0);
    // }

    // // Très important pour passer a la prochain subpass
    // vkCmdNextSubpass(m_commandBuffers.at(i), VK_SUBPASS_CONTENTS_INLINE);

    {
      vkCmdBindPipeline(m_commandBuffers.at(i), VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline.pipeline());
      vkCmdBindDescriptorSets(m_commandBuffers.at(i), VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline.layout(), 0,
                              1, &(m_descriptorSets.descriptor(i)), 0, nullptr);

      const Buffer<Vertex>* vertexBuffer = dynamic_cast<const Buffer<Vertex>*>(m_buffers[0]);
      const VkBuffer vertexBuffers[]     = {vertexBuffer->buffer()};
      const VkDeviceSize offsets[]       = {0};
      vkCmdBindVertexBuffers(m_commandBuffers.at(i), 0, 1, vertexBuffers, offsets);
      vkCmdDraw(m_commandBuffers.at(i), static_cast<uint32_t>(vertexBuffer->data().size()), 1, 0, 0);
    }

    vkCmdEndRenderPass(m_commandBuffers.at(i));

    if (vkEndCommandBuffer(m_commandBuffers.at(i)) != VK_SUCCESS) {
      throw std::runtime_error("failed to record command buffer!");
    }
  }
}
