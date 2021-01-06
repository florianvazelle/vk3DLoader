#include <vkl/Depth/DepthCommandBuffers.hpp>

#include <stdexcept>

#ifndef SHADOWMAP_DIM
#  define SHADOWMAP_DIM 2048
#endif

using namespace vkl;

DepthCommandBuffers::DepthCommandBuffers(const Device& device,
                                         const RenderPass& renderPass,
                                         const SwapChain& swapChain,
                                         const GraphicsPipeline& graphicsPipeline,
                                         const CommandPool& commandPool,
                                         const VertexBuffer& vertexBuffer,
                                         const DescriptorSets& descriptorSets)
    : CommandBuffers(device, renderPass, swapChain, graphicsPipeline, commandPool),
      m_vertexBuffer(vertexBuffer),
      m_descriptorSets(descriptorSets) {
  createCommandBuffers();
}

void DepthCommandBuffers::recreate() {
  destroyCommandBuffers();
  createCommandBuffers();
}

void DepthCommandBuffers::createCommandBuffers() {
  m_commandBuffers.resize(m_renderPass.size());

  const VkCommandBufferAllocateInfo allocInfo = {
      .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .commandPool        = m_commandPool.handle(),
      .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = (uint32_t)m_commandBuffers.size(),
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

  {
    VkClearValue clearValues[2];
    clearValues[0].depthStencil = {1.0f, 0};

    const VkRenderPassBeginInfo renderPassBeginInfo = {
          .sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
          .renderPass        = m_renderPass.handle(),
          .framebuffer       = m_renderPass.frameBuffer(bufferIdx),
          .renderArea = {
              .offset = {0, 0},
              .extent = {
                 .width = SHADOWMAP_DIM,
                 .height = SHADOWMAP_DIM,
              },
          },
          .clearValueCount   = 1,
          .pClearValues      = clearValues,
      };

    vkCmdBeginRenderPass(m_commandBuffers.at(bufferIdx), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport = {
        .width    = SHADOWMAP_DIM,
        .height   = SHADOWMAP_DIM,
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };
    vkCmdSetViewport(m_commandBuffers.at(bufferIdx), 0, 1, &viewport);

    VkRect2D scissor = {
          .offset = {
              .x = 0,
              .y = 0,
          },
          .extent = {
             .width  = SHADOWMAP_DIM,
             .height = SHADOWMAP_DIM,
          },
      };
    vkCmdSetScissor(m_commandBuffers.at(bufferIdx), 0, 1, &scissor);

    // Set depth bias (aka "Polygon offset")
    // Required to avoid shadow mapping artifacts
    vkCmdSetDepthBias(m_commandBuffers.at(bufferIdx), m_depthBiasConstant, 0.0f, m_depthBiasSlope);

    vkCmdBindPipeline(m_commandBuffers.at(bufferIdx), VK_PIPELINE_BIND_POINT_GRAPHICS,
                      m_graphicsPipeline.depthPipeline());
    vkCmdBindDescriptorSets(m_commandBuffers.at(bufferIdx), VK_PIPELINE_BIND_POINT_GRAPHICS,
                            m_graphicsPipeline.layout(), 0, 1, &(m_descriptorSets.depthDescriptor(bufferIdx)), 0,
                            nullptr);

    const VkBuffer vertexBuffers[] = {m_vertexBuffer.buffer()};
    const VkDeviceSize offsets[]   = {0};
    vkCmdBindVertexBuffers(m_commandBuffers.at(bufferIdx), 0, 1, vertexBuffers, offsets);

    vkCmdDraw(m_commandBuffers.at(bufferIdx), static_cast<uint32_t>(m_vertexBuffer.size()), 1, 0, 0);

    vkCmdEndRenderPass(m_commandBuffers.at(bufferIdx));
  }

  if (vkEndCommandBuffer(m_commandBuffers.at(bufferIdx)) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer!");
  }
}