#include <vkl/CommandBuffers.hpp>
#include <vkl/CommandPool.hpp>
#include <vkl/Device.hpp>
#include <vkl/GraphicsPipeline.hpp>
#include <vkl/RenderPass.hpp>
#include <vkl/SwapChain.hpp>

#include <stdexcept>

using namespace vkl;

CommandBuffers::CommandBuffers(const Device& device,
                               const RenderPass& renderPass,
                               const SwapChain& swapChain,
                               const GraphicsPipeline& graphicsPipeline,
                               const CommandPool& commandPool)
    : m_device(device),
      m_renderPass(renderPass),
      m_swapChain(swapChain),
      m_graphicsPipeline(graphicsPipeline),
      m_commandPool(commandPool) {}

CommandBuffers::~CommandBuffers() { destroyCommandBuffers(); }

void CommandBuffers::destroyCommandBuffers() {
  vkFreeCommandBuffers(m_device.logical(), m_commandPool.handle(), static_cast<uint32_t>(m_commandBuffers.size()),
                       m_commandBuffers.data());
}

void CommandBuffers::SingleTimeCommands(const Device& device,
                                        const CommandPool& cmdPool,
                                        const std::function<void(const VkCommandBuffer&)>& func) {
  const VkCommandBufferAllocateInfo allocInfo = {
      .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .commandPool        = cmdPool.handle(),
      .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = 1,
  };

  VkCommandBuffer commandBuffer = {};
  if (vkAllocateCommandBuffers(device.logical(), &allocInfo, &commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }

  const VkCommandBufferBeginInfo beginInfo = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
  };

  if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error("Could not create one-time command buffer!");
  }

  func(commandBuffer);

  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer!");
  }

  const VkSubmitInfo submitInfo = {
      .sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .commandBufferCount = 1,
      .pCommandBuffers    = &commandBuffer,
  };

  vkQueueSubmit(device.graphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(device.graphicsQueue());

  vkFreeCommandBuffers(device.logical(), cmdPool.handle(), 1, &commandBuffer);
}