// clang-format off
#include <particle/Compute/ComputeCommandBuffer.hpp>
#include <stdexcept>                             // for runtime_error
#include <common/CommandPool.hpp>                // for CommandPool, vkl
#include <common/DescriptorSets.hpp>             // for DescriptorSets
#include <common/Device.hpp>                     // for Device
#include <common/buffer/StorageBuffer.hpp>       // for StorageBuffer
#include <particle/Compute/ComputePipeline.hpp>  // for ComputePipeline
#include <common/RenderPass.hpp>                // for IRenderPass
#include <common/SwapChain.hpp>                  // for SwapChain
// clang-format on

#ifndef NUM_PARTICLE
#  define NUM_PARTICLE 10
#endif

using namespace vkl;

ComputeCommandBuffer::ComputeCommandBuffer(const Device& device,
                                           const IRenderPass& renderPass,
                                           const SwapChain& swapChain,
                                           const ComputePipeline& computePipeline,
                                           const StorageBuffer& storageBuffer,
                                           const CommandPool& commandPool,
                                           const DescriptorSets& descriptorSets)
    : m_device(device),
      m_renderPass(renderPass),
      m_swapChain(swapChain),
      m_computePipeline(computePipeline),
      m_commandPool(commandPool),
      m_storageBuffer(storageBuffer),
      m_descriptorSets(descriptorSets) {
  createCommandBuffers();
}

void ComputeCommandBuffer::recreate() {
  destroyCommandBuffers();
  createCommandBuffers();
}

void ComputeCommandBuffer::destroyCommandBuffers() {
  vkFreeCommandBuffers(m_device.logical(), m_commandPool.handle(), 1, &m_commandBuffer);
}

void ComputeCommandBuffer::createCommandBuffers() {
  const VkCommandBufferAllocateInfo allocInfo = {
      .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .commandPool        = m_commandPool.handle(),
      .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = 1,
  };

  if (vkAllocateCommandBuffers(m_device.logical(), &allocInfo, &m_commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }

  const VkCommandBufferBeginInfo cmdBufInfo = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
  };

  if (vkBeginCommandBuffer(m_commandBuffer, &cmdBufInfo) != VK_SUCCESS) {
    throw std::runtime_error("failed to begin recording command buffer!");
  }

  // Acquire barrier
  // if (graphics.queueFamilyIndex != compute.queueFamilyIndex) {
  //   VkBufferMemoryBarrier buffer_barrier = {VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
  //                                           nullptr,
  //                                           0,
  //                                           VK_ACCESS_SHADER_WRITE_BIT,
  //                                           graphics.queueFamilyIndex,
  //                                           compute.queueFamilyIndex,
  //                                           m_storageBuffer.buffer(),
  //                                           0,
  //                                           m_storageBuffer.size()};

  //   vkCmdPipelineBarrier(m_commandBuffer, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
  //                        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, nullptr, 1, &buffer_barrier, 0, nullptr);
  // }

  // First pass: Calculate particle movement
  // -------------------------------------------------------------------------------------------------------
  vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_computePipeline.pipelineCalculate());
  vkCmdBindDescriptorSets(m_commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_computePipeline.layout(), 0, 1,
                          &m_descriptorSets.descriptor(0), 0, 0);
  vkCmdDispatch(m_commandBuffer, NUM_PARTICLE / 256, 1, 1);

  // Add memory barrier to ensure that the computer shader has finished writing to the buffer
  VkBufferMemoryBarrier bufferBarrier = {
      .sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
  };
  bufferBarrier.buffer = m_storageBuffer.buffer();
  // bufferBarrier.size                  = m_storageBuffer.descriptor.range;
  bufferBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
  bufferBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
  // Transfer ownership if compute and graphics queue family indices differ
  bufferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  bufferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

  vkCmdPipelineBarrier(m_commandBuffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0,
                       0, nullptr, 1, &bufferBarrier, 0, nullptr);

  // Second pass: Integrate particles
  // -------------------------------------------------------------------------------------------------------
  vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_computePipeline.pipelineIntegrate());
  vkCmdDispatch(m_commandBuffer, NUM_PARTICLE / 256, 1, 1);

  // Release barrier
  // if (graphics.queueFamilyIndex != compute.queueFamilyIndex) {
  //   VkBufferMemoryBarrier buffer_barrier = {VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
  //                                           nullptr,
  //                                           VK_ACCESS_SHADER_WRITE_BIT,
  //                                           0,
  //                                           compute.queueFamilyIndex,
  //                                           graphics.queueFamilyIndex,
  //                                           m_storageBuffer.buffer(),
  //                                           0,
  //                                           m_storageBuffer.size()};

  //   vkCmdPipelineBarrier(m_commandBuffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
  //                        VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, 0, nullptr, 1, &buffer_barrier, 0, nullptr);
  // }

  if (vkEndCommandBuffer(m_commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer!");
  }
}