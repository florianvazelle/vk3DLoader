#ifndef COMPUTECOMMANDBUFFER_HPP
#define COMPUTECOMMANDBUFFER_HPP

// clang-format off
#include <vulkan/vulkan_core.h>  // for VkCommandBuffer, VkCommandBuffer_T
#include <NoCopy.hpp>       // for NoCopy
namespace vkl { class CommandPool; }
namespace vkl { class ComputePipeline; }
namespace vkl { class DescriptorSets; }
namespace vkl { class Device; }
namespace vkl { class IRenderPass; }
namespace vkl { class StorageBuffer; }
namespace vkl { class SwapChain; }
// clang-format on

namespace vkl {

  class ComputeCommandBuffer : public NoCopy {
  public:
    ComputeCommandBuffer(const Device& device,
                         const IRenderPass& renderPass,
                         const SwapChain& swapChain,
                         const ComputePipeline& computePipeline,
                         const StorageBuffer& storageBuffer,
                         const CommandPool& commandPool,
                         const DescriptorSets& descriptorSets);
    void recreate();

    inline VkCommandBuffer& command() { return m_commandBuffer; }
    inline const VkCommandBuffer& command() const { return m_commandBuffer; }

  protected:
    VkCommandBuffer m_commandBuffer;

    const Device& m_device;
    const IRenderPass& m_renderPass;
    const SwapChain& m_swapChain;
    const ComputePipeline& m_computePipeline;
    const StorageBuffer& m_storageBuffer;
    const CommandPool& m_commandPool;
    const DescriptorSets& m_descriptorSets;

    void createCommandBuffers();
    void destroyCommandBuffers();

    // allocate one command buffer
    VkCommandBuffer allocCommandBuffer(VkCommandBufferLevel level, VkCommandPool pool, bool begin = false) const;
  };

}  // namespace vkl

#endif  // COMPUTECOMMANDBUFFER_HPP
