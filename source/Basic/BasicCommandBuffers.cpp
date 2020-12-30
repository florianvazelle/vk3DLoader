#include <vkl/Basic/BasicCommandBuffers.hpp>

#include <stdexcept>

using namespace vkl;

BasicCommandBuffers::BasicCommandBuffers(const Device& device,
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

void BasicCommandBuffers::recreate() {
  destroyCommandBuffers();
  createCommandBuffers();
}

void BasicCommandBuffers::createCommandBuffers() {
  m_commandBuffers.resize(m_renderPass.size());

  VkCommandBufferAllocateInfo allocInfo = {
      .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .commandPool        = m_commandPool.handle(),
      .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = (uint32_t)m_commandBuffers.size(),
  };

  if (vkAllocateCommandBuffers(m_device.logical(), &allocInfo, m_commandBuffers.data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }

  for (size_t i = 0; i < m_commandBuffers.size(); i++) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(m_commandBuffers[i], &beginInfo) != VK_SUCCESS) {
      throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};

    VkRenderPassBeginInfo renderPassInfo = {
        .sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass        = m_renderPass.handle(),
        .framebuffer       = m_renderPass.frameBuffer(i),
        .renderArea = {
            .offset = {0, 0},
            .extent = m_swapChain.extent(),
        },
        .clearValueCount   = 1,
        .pClearValues      = &clearColor,
    };

    vkCmdBeginRenderPass(m_commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(m_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline.pipeline());

    const VkBuffer vertexBuffers[] = {m_vertexBuffer.buffer()};
    const VkDeviceSize offsets[]   = {0};
    vkCmdBindVertexBuffers(m_commandBuffers[i], 0, 1, vertexBuffers, offsets);

    const VkDescriptorSet descriptors[] = {m_descriptorSets.descriptor(i)};
    vkCmdBindDescriptorSets(m_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline.layout(), 0, 1,
                            descriptors, 0, nullptr);

    vkCmdDraw(m_commandBuffers[i], static_cast<uint32_t>(m_vertexBuffer.size()), 1, 0, 0);

    vkCmdEndRenderPass(m_commandBuffers[i]);

    if (vkEndCommandBuffer(m_commandBuffers[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to record command buffer!");
    }
  }
}