#ifndef BASICCOMMANDBUFFERS_HPP
#define BASICCOMMANDBUFFERS_HPP

#include <vulkan/vulkan.h>
#include <vector>

#include <vkl/CommandBuffers.hpp>
#include <vkl/DescriptorSets.hpp>
#include <vkl/buffer/Buffer.hpp>

namespace vkl {

  class BasicCommandBuffers : public CommandBuffers {
  public:
    BasicCommandBuffers(const Device& device,
                        const BasicRenderPass& m_basicRenderPass,
                        const SwapChain& swapChain,
                        const GraphicsPipeline& graphicsPipeline,
                        const CommandPool& commandPool,
                        const VertexBuffer& vertexBuffer,
                        const DescriptorSets& descriptorSets);
    void recreate();

  private:
    void createCommandBuffers();

    const BasicRenderPass& m_basicRenderPass;
    const VertexBuffer& m_vertexBuffer;
    const DescriptorSets& m_descriptorSets;
  };

}  // namespace vkl

#endif  // BASICCOMMANDBUFFERS_HPP