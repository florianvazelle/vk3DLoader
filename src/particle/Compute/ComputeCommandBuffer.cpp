// clang-format off
#include <particle/Compute/ComputeCommandBuffer.hpp>
#include <stdexcept>                             // for runtime_error
#include <common/CommandPool.hpp>                // for CommandPool, vkl
#include <common/DescriptorSets.hpp>             // for DescriptorSets
#include <common/Device.hpp>                     // for Device
#include <common/buffer/StorageBuffer.hpp>       // for StorageBuffer
#include <particle/Compute/ComputePipeline.hpp>  // for ComputePipeline
#include <common/RenderPass.hpp>                // for IRenderPass
#include <common/Semaphore.hpp>                  // for SwapChain
#include <common/SwapChain.hpp>                  // for SwapChain
#include <common/Device.hpp>
// clang-format on

#ifndef NUM_PARTICLE
#  define NUM_PARTICLE 10
#endif

#define DEFAULT_FENCE_TIMEOUT 100000000000

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

void ComputeCommandBuffer::flushCommandBuffer(VkCommandBuffer cmdBuffer,
                                              VkQueue queue,
                                              VkCommandPool pool,
                                              bool free) const {
  if (cmdBuffer == VK_NULL_HANDLE) {
    return;
  }

  vkEndCommandBuffer(cmdBuffer);

  const VkSubmitInfo submitInfo = {
      .sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .commandBufferCount = 1,
      .pCommandBuffers    = &cmdBuffer,
  };

  // Create fence to ensure that the command buffer has finished executing
  const VkFenceCreateInfo fenceInfo = {
      .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
  };
  VkFence fence;
  vkCreateFence(m_device.logical(), &fenceInfo, nullptr, &fence);

  // Submit to the queue
  vkQueueSubmit(queue, 1, &submitInfo, fence);

  // Wait for the fence to signal that command buffer has finished executing
  vkWaitForFences(m_device.logical(), 1, &fence, VK_TRUE, DEFAULT_FENCE_TIMEOUT);
  vkDestroyFence(m_device.logical(), fence, nullptr);

  if (free) {
    vkFreeCommandBuffers(m_device.logical(), pool, 1, &cmdBuffer);
  }
}

void ComputeCommandBuffer::createCommandBuffers() {
  {
    const VkSubmitInfo submitInfo = {
        .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores    = &m_semaphore.handle(),
    };
    vkQueueSubmit(m_device.computeQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_device.computeQueue());
  }

  const std::optional<uint32_t>& queueGraphicFamilyIndex = m_device.queueFamilyIndices().graphicsFamily;
  const std::optional<uint32_t>& queueComputeFamilyIndex = m_device.queueFamilyIndices().computeFamily;

  // Build a single command buffer containing the compute dispatch commands
  {
    m_commandBuffer = allocCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, m_commandPool.handle(), true);


    // Acquire barrier
    if (queueGraphicFamilyIndex.value() != queueComputeFamilyIndex.value()) {
      VkBufferMemoryBarrier buffer_barrier = {
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
                           0, nullptr, 1, &buffer_barrier, 0, nullptr);
    }

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
    if (queueGraphicFamilyIndex.value() != queueComputeFamilyIndex.value()) {
      VkBufferMemoryBarrier buffer_barrier = {
          VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
          nullptr,
          VK_ACCESS_SHADER_WRITE_BIT,
          0,
          queueComputeFamilyIndex.value(),
          queueGraphicFamilyIndex.value(),
          m_storageBuffer.buffer(),
          0,
          m_storageBuffer.size(),
      };

      vkCmdPipelineBarrier(m_commandBuffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0,
                           0, nullptr, 1, &buffer_barrier, 0, nullptr);
    }

    if (vkEndCommandBuffer(m_commandBuffer) != VK_SUCCESS) {
      throw std::runtime_error("failed to record command buffer!");
    }
  }

  // If graphics and compute queue family indices differ, acquire and immediately release the storage buffer, so that
  // the initial acquire from the graphics command buffers are matched up properly
  if (queueGraphicFamilyIndex.value() != queueComputeFamilyIndex.value()) {
    // Create a transient command buffer for setting up the initial buffer transfer state
    const VkCommandBufferAllocateInfo allocInfo = {
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool        = m_commandPool.handle(),
        .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };
    VkCommandBuffer transferCmd = m_commandBuffer
        = allocCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, m_commandPool.handle(), true);

    VkBufferMemoryBarrier acquire_buffer_barrier = {
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
    vkCmdPipelineBarrier(transferCmd, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0,
                         nullptr, 1, &acquire_buffer_barrier, 0, nullptr);

    VkBufferMemoryBarrier release_buffer_barrier = {
        VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
        nullptr,
        VK_ACCESS_SHADER_WRITE_BIT,
        0,
        queueComputeFamilyIndex.value(),
        queueGraphicFamilyIndex.value(),
        m_storageBuffer.buffer(),
        0,
        m_storageBuffer.size(),
    };
    vkCmdPipelineBarrier(transferCmd, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, 0,
                         nullptr, 1, &release_buffer_barrier, 0, nullptr);

    flushCommandBuffer(transferCmd, m_device.computeQueue(), m_commandPool.handle());
  }
}
