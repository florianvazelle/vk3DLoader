#ifndef DEPTHCOMMANDBUFFERS_HPP
#define DEPTHCOMMANDBUFFERS_HPP

#include <stdint.h>                   
#include <cassert>                   
#include <vector>     
#include <poike/poike.hpp>

using namespace poike;

namespace vkl {

  class DepthCommandBuffers : public CommandBuffers {
  public:
    DepthCommandBuffers(const Device& device,
                        const RenderPass& renderPass,
                        const SwapChain& swapChain,
                        const GraphicsPipeline& graphicsPipeline,
                        const CommandPool& commandPool,
                        const DescriptorSets& descriptorSets,
                        const std::vector<const IBuffer*>& buffers)
        : CommandBuffers(device, renderPass, swapChain, graphicsPipeline, commandPool, descriptorSets, buffers) {
      createCommandBuffers();
    }

    void recordCommandBuffers(uint32_t bufferIdx);

    float& depthBiasConstant() { return m_depthBiasConstant; }
    float& depthBiasSlope() { return m_depthBiasSlope; }

  private:
    // Depth bias (and slope) are used to avoid shadowing artifacts
    // Constant depth bias factor (always applied)
    float m_depthBiasConstant = 1.25f;
    // Slope depth bias factor, applied depending on polygon's slope
    float m_depthBiasSlope = 1.75f;

    void createCommandBuffers() final;
  };

}  // namespace vkl

#endif  // DEPTHCOMMANDBUFFERS_HPP
