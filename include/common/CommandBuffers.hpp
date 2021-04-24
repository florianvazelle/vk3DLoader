/**
 * @file CommandBuffers.hpp
 * @brief Define CommandBuffers base class
 */

#ifndef COMMANDBUFFERS_HPP
#define COMMANDBUFFERS_HPP

#include <vulkan/vulkan.h>
#include <NonCopyable.hpp>
#include <common/CommandPool.hpp>
#include <common/Device.hpp>
#include <common/GraphicsPipeline.hpp>
#include <common/RenderPass.hpp>
#include <common/SwapChain.hpp>
#include <functional>
#include <vector>

namespace vkl {

  class CommandBuffers : public NonCopyable {
  public:
    CommandBuffers(const Device& device,
                   const RenderPass& renderpass,
                   const SwapChain& swapChain,
                   const GraphicsPipeline& graphicsPipeline,
                   const CommandPool& commandPool);
    ~CommandBuffers();

    inline VkCommandBuffer& command(uint32_t index) { return m_commandBuffers[index]; }
    inline const VkCommandBuffer& command(uint32_t index) const { return m_commandBuffers[index]; }

    virtual void recreate() = 0;
    static void SingleTimeCommands(const Device& device,
                                   const CommandPool& cmdPool,
                                   const std::function<void(const VkCommandBuffer&)>& func);

  protected:
    std::vector<VkCommandBuffer> m_commandBuffers;

    const Device& m_device;
    const RenderPass& m_renderPass;
    const SwapChain& m_swapChain;
    const GraphicsPipeline& m_graphicsPipeline;
    const CommandPool& m_commandPool;

    virtual void createCommandBuffers() = 0;
    void destroyCommandBuffers();
  };
}  // namespace vkl

#endif  // COMMANDBUFFERS_HPP