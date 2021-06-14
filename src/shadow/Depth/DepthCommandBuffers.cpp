// clang-format off
#include <shadow/Depth/DepthCommandBuffers.hpp>
#include <vulkan/vulkan_core.h>         // for VkCommandBuffer, VkCommandBuf...
#include <stdexcept>                    // for runtime_error
#include <common/CommandPool.hpp>       // for CommandPool, vkl
#include <common/DescriptorSets.hpp>    // for DescriptorSets
#include <common/Device.hpp>            // for Device
#include <common/GraphicsPipeline.hpp>  // for GraphicsPipeline
#include <common/RenderPass.hpp>        // for RenderPass
#include <common/SwapChain.hpp>         // for SwapChain
#include <common/buffer/Buffer.hpp>     // for Buffer
#include <common/buffer/IBuffer.hpp>    // for IBuffer
#include <common/struct/Vertex.hpp>     // for Vertex
// clang-format on

#ifndef SHADOWMAP_DIM
#  define SHADOWMAP_DIM 2048
#endif

using namespace vkl;

void DepthCommandBuffers::createCommandBuffers() {
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
}

void DepthCommandBuffers::recordCommandBuffers(uint32_t bufferIdx) {
  const VkCommandBufferBeginInfo beginInfo = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
  };

  if (vkBeginCommandBuffer(m_commandBuffers.at(bufferIdx), &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error("failed to begin recording command buffer!");
  }

  VkClearValue clearValues[2];
  clearValues[0].depthStencil = {1.0f, 0};

  const VkRenderPassBeginInfo renderPassBeginInfo = {
          .sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
          .renderPass        = m_renderPass.handle(),
          .framebuffer       = m_renderPass.frameBuffer(bufferIdx),
          .renderArea = {
              .offset = {0, 0},
              .extent = m_swapChain.extent(),
          },
          .clearValueCount   = 1,
          .pClearValues      = clearValues,
      };

  vkCmdBeginRenderPass(m_commandBuffers.at(bufferIdx), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

  {
    // Set depth bias (aka "Polygon offset")
    // Required to avoid shadow mapping artifacts
    vkCmdSetDepthBias(m_commandBuffers.at(bufferIdx), m_depthBiasConstant, 0.0f, m_depthBiasSlope);

    vkCmdBindPipeline(m_commandBuffers.at(bufferIdx), VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline.pipeline());
    vkCmdBindDescriptorSets(m_commandBuffers.at(bufferIdx), VK_PIPELINE_BIND_POINT_GRAPHICS,
                            m_graphicsPipeline.layout(), 0, 1, &(m_descriptorSets.descriptor(bufferIdx)), 0, nullptr);

    const Buffer<Vertex>* vertexBuffer = dynamic_cast<const Buffer<Vertex>*>(m_buffers[0]);

    const VkBuffer vertexBuffers[] = {vertexBuffer->buffer()};
    const VkDeviceSize offsets[]   = {0};
    vkCmdBindVertexBuffers(m_commandBuffers.at(bufferIdx), 0, 1, vertexBuffers, offsets);
    vkCmdDraw(m_commandBuffers.at(bufferIdx), static_cast<uint32_t>(vertexBuffer->data().size()), 1, 0, 0);
    vkCmdEndRenderPass(m_commandBuffers.at(bufferIdx));
  }

  if (vkEndCommandBuffer(m_commandBuffers.at(bufferIdx)) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer!");
  }
}
