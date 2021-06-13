// clang-format off
#include <particle/Compute/ComputeCommandBuffer.hpp>
#include <stdexcept>                             // for runtime_error
#include <common/CommandPool.hpp>                // for CommandPool, vkl
#include <common/DescriptorSets.hpp>             // for DescriptorSets
#include <common/Device.hpp>                     // for Device
#include <common/buffer/StorageBuffer.hpp>       // for StorageBuffer
#include <common/struct/Particle.hpp>            // for Particle
#include <particle/Compute/ComputePipeline.hpp>  // for ComputePipeline
#include <common/RenderPass.hpp>                 // for IRenderPass
#include <common/Semaphore.hpp>                  // for SwapChain
#include <common/SwapChain.hpp>                  // for SwapChain
#include <common/Device.hpp>
// clang-format on

#ifndef NUM_PARTICLE
#  define NUM_PARTICLE 10
#endif

#define DEFAULT_FENCE_TIMEOUT 100000000000

// https://community.khronos.org/t/why-i-am-getting-this-validator-message-memory-buffer-barrier/106638

using namespace vkl;

ComputeCommandBuffer::ComputeCommandBuffer(const Device& device,
                                           const IRenderPass& renderPass,
                                           const SwapChain& swapChain,
                                           const ComputePipeline& computePipeline,
                                           const StorageBuffer& storageBuffer,
                                           const CommandPool& commandPool,
                                           const Semaphore& semaphore,
                                           const DescriptorSets& descriptorSets)
    : m_device(device),
      m_renderPass(renderPass),
      m_swapChain(swapChain),
      m_computePipeline(computePipeline),
      m_storageBuffer(storageBuffer),
      m_commandPool(commandPool),
      m_semaphore(semaphore),
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

VkCommandBuffer ComputeCommandBuffer::allocCommandBuffer(VkCommandBufferLevel level,
                                                         VkCommandPool pool,
                                                         bool begin) const {
  VkCommandBuffer cmdBuffer;

  const VkCommandBufferAllocateInfo allocInfo = {
      .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .commandPool        = pool,
      .level              = level,
      .commandBufferCount = 1,
  };

  if (vkAllocateCommandBuffers(m_device.logical(), &allocInfo, &cmdBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }

  // If requested, also start recording for the new command buffer
  if (begin) {
    const VkCommandBufferBeginInfo cmdBufInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    };

    if (vkBeginCommandBuffer(cmdBuffer, &cmdBufInfo) != VK_SUCCESS) {
      throw std::runtime_error("failed to begin recording command buffer!");
    }
  }

  return cmdBuffer;
}

void ComputeCommandBuffer::createCommandBuffers() {
  {
    const VkSubmitInfo submitInfo = {
        .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores    = &m_semaphore.handle(),
    };

    // maybe graphics queue
    if (vkQueueSubmit(m_device.graphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
      throw std::runtime_error("failed to submit signal semaphore!");
    }
    vkQueueWaitIdle(m_device.computeQueue());
  }

  const std::optional<uint32_t>& queueGraphicFamilyIndex = m_device.queueFamilyIndices().graphicsFamily;
  const std::optional<uint32_t>& queueComputeFamilyIndex = m_device.queueFamilyIndices().computeFamily;

  // Build a single command buffer containing the compute dispatch commands
  m_commandBuffer = allocCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, m_commandPool.handle(), true);

  // Here, we need barrier if we doen't have graphic and compute on the same queue

  // Acquire barrier
  if (queueGraphicFamilyIndex.value() != queueComputeFamilyIndex.value()) {
    const VkBufferMemoryBarrier acquire_barrier = {
        VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
        nullptr,
        0,
        VK_ACCESS_SHADER_WRITE_BIT,
        queueGraphicFamilyIndex.value(),
        queueComputeFamilyIndex.value(),
        m_storageBuffer.buffer(),
        0,
        m_storageBuffer.size(),
    };

    vkCmdPipelineBarrier(m_commandBuffer, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0,
                         0, nullptr, 1, &acquire_barrier, 0, nullptr);
  }

  // First pass: Calculate particle movement
  // -------------------------------------------------------------------------------------------------------
  vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_computePipeline.pipelineCalculate());
  vkCmdBindDescriptorSets(m_commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_computePipeline.layout(), 0, 1,
                          &m_descriptorSets.descriptor(0), 0, 0);
  vkCmdDispatch(m_commandBuffer, NUM_PARTICLE / 256, 1, 1);

  // Add memory barrier to ensure that the computer shader has finished writing to the buffer
  const VkBufferMemoryBarrier bufferBarrier = {
      .sType         = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
      .srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
      .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
      // Transfer ownership if compute and graphics queue family indices differ
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .buffer              = m_storageBuffer.buffer(),
      .size                = m_storageBuffer.descriptor().range,
  };

  vkCmdPipelineBarrier(m_commandBuffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0,
                       0, nullptr, 1, &bufferBarrier, 0, nullptr);

  // Second pass: Integrate particles
  // -------------------------------------------------------------------------------------------------------
  vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_computePipeline.pipelineIntegrate());
  vkCmdDispatch(m_commandBuffer, NUM_PARTICLE / 256, 1, 1);

  // Release barrier
  if (queueGraphicFamilyIndex.value() != queueComputeFamilyIndex.value()) {
    const VkBufferMemoryBarrier release_barrier = {
        .sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
        .srcAccessMask       = VK_ACCESS_SHADER_WRITE_BIT,
        .dstAccessMask       = 0,
        .srcQueueFamilyIndex = queueComputeFamilyIndex.value(),
        .dstQueueFamilyIndex = queueGraphicFamilyIndex.value(),
        .buffer              = m_storageBuffer.buffer(),
        .offset              = 0,
        .size                = m_storageBuffer.size(),
    };

    vkCmdPipelineBarrier(m_commandBuffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0,
                         0, nullptr, 1, &release_barrier, 0, nullptr);
  }

  if (vkEndCommandBuffer(m_commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer!");
  }
}
