#ifndef DEPTHCOMMANDBUFFERS_HPP
#define DEPTHCOMMANDBUFFERS_HPP

#include <vulkan/vulkan.h>
#include <vector>

#include <vkl/CommandBuffers.hpp>
#include <vkl/Depth/DepthRenderPass.hpp>
#include <vkl/DescriptorSets.hpp>
#include <vkl/buffer/VertexBuffer.hpp>

namespace vkl {

  class DepthCommandBuffers : public CommandBuffers {
  public:
    DepthCommandBuffers(const Device& device,
                        const DepthRenderPass& depthRenderpass,
                        const SwapChain& swapChain,
                        const GraphicsPipeline& graphicsPipeline,
                        const CommandPool& commandPool,
                        const VertexBuffer& vertexBuffer,
                        const DescriptorSets& descriptorSets);
    void recreate();
    void recordCommandBuffers(uint32_t bufferIdx);

    float& depthBiasConstant() { return m_depthBiasConstant; }
    float& depthBiasSlope() { return m_depthBiasSlope; }

  private:
    void createCommandBuffers();

    // Depth bias (and slope) are used to avoid shadowing artifacts
    // Constant depth bias factor (always applied)
    float m_depthBiasConstant = 1.25f;
    // Slope depth bias factor, applied depending on polygon's slope
    float m_depthBiasSlope = 1.75f;

    const DepthRenderPass& m_depthRenderpass;
    const VertexBuffer& m_vertexBuffer;
    const DescriptorSets& m_descriptorSets;
  };

}  // namespace vkl

#endif  // DEPTHCOMMANDBUFFERS_HPP