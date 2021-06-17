// clang-format off
#include <particle/Compute/ComputeCommandBuffer.hpp>
#include <stdexcept>                             // for runtime_error
#include <common/CommandPool.hpp>                // for CommandPool, vkl
#include <common/DescriptorSets.hpp>             // for DescriptorSets
#include <common/Device.hpp>                     // for Device
#include <common/buffer/StorageBuffer.hpp>       // for StorageBuffer
#include <common/struct/Particle.hpp>            // for Particle
#include <particle/Compute/ComputePipeline.hpp>  // for ComputePipeline
#include <common/RenderPass.hpp>                 // for RenderPass
#include <common/CommandBuffers.hpp>
#include <common/Device.hpp>
#include <common/Semaphore.hpp>
// clang-format on

#ifndef NUM_PARTICLE
#  define NUM_PARTICLE 10
#endif

// https://community.khronos.org/t/why-i-am-getting-this-validator-message-memory-buffer-barrier/106638

using namespace vkl;

ComputeCommandBuffer::ComputeCommandBuffer(const Device& device,
                                           const RenderPass& renderPass,
                                           const ComputePipeline& computePipeline,
                                           const StorageBuffer& storageBuffer,
                                           const CommandPool& commandPool,
                                           const DescriptorSets& descriptorSets)
    : m_device(device),
      m_renderPass(renderPass),
      m_computePipeline(computePipeline),
      m_storageBuffer(storageBuffer),
      m_commandPool(commandPool),
      m_descriptorSets(descriptorSets) {


  createCommandBuffers();

  const std::optional<uint32_t>& graphicsFamily = device.queueFamilyIndices().graphicsFamily;
  const std::optional<uint32_t>& computeFamily  = device.queueFamilyIndices().computeFamily;


  // Transfer
  if (graphicsFamily.value() != computeFamily.value()) {
    CommandBuffers::SingleTimeCommands(
        m_device, m_commandPool, m_device.computeQueue(), [&](const VkCommandBuffer& cmdBuffer) {
          const VkBufferMemoryBarrier acquire_buffer_barrier = {
              .sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
              .srcAccessMask       = 0,
              .dstAccessMask       = VK_ACCESS_SHADER_WRITE_BIT,
              .srcQueueFamilyIndex = graphicsFamily.value(),
              .dstQueueFamilyIndex = computeFamily.value(),
              .buffer              = m_storageBuffer.buffer(),
              .offset              = 0,
              .size                = m_storageBuffer.size(),
          };

          vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0,
                               0, nullptr, 1, &acquire_buffer_barrier, 0, nullptr);

          const VkBufferMemoryBarrier release_buffer_barrier = {
              .sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
              .srcAccessMask       = VK_ACCESS_SHADER_WRITE_BIT,
              .dstAccessMask       = 0,
              .srcQueueFamilyIndex = computeFamily.value(),
              .dstQueueFamilyIndex = graphicsFamily.value(),
              .buffer              = m_storageBuffer.buffer(),
              .offset              = 0,
              .size                = m_storageBuffer.size(),
          };

          vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0,
                               0, nullptr, 1, &release_buffer_barrier, 0, nullptr);
        });
  }
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
  const std::optional<uint32_t>& graphicsFamily = m_device.queueFamilyIndices().graphicsFamily;
  const std::optional<uint32_t>& computeFamily  = m_device.queueFamilyIndices().computeFamily;

  // Build a single command buffer containing the compute dispatch commands
  m_commandBuffer = allocCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, m_commandPool.handle(), true);

  // Here, we need barrier if we doen't have graphic and compute on the same queue

  // Acquire barrier
  if (graphicsFamily.value() != computeFamily.value()) {
    const VkBufferMemoryBarrier acquire_barrier = {
        .sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
        .srcAccessMask       = 0,
        .dstAccessMask       = VK_ACCESS_SHADER_WRITE_BIT,
        .srcQueueFamilyIndex = graphicsFamily.value(),
        .dstQueueFamilyIndex = computeFamily.value(),
        .buffer              = m_storageBuffer.buffer(),
        .offset              = 0,
        .size                = m_storageBuffer.size(),
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
  if (graphicsFamily.value() != computeFamily.value()) {
    const VkBufferMemoryBarrier release_barrier = {
        .sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
        .srcAccessMask       = VK_ACCESS_SHADER_WRITE_BIT,
        .dstAccessMask       = 0,
        .srcQueueFamilyIndex = computeFamily.value(),
        .dstQueueFamilyIndex = graphicsFamily.value(),
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
