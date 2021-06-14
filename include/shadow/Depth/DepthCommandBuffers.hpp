#ifndef DEPTHCOMMANDBUFFERS_HPP
#define DEPTHCOMMANDBUFFERS_HPP

// clang-format off
#include <stdint.h>                   // for uint32_t
#include <cassert>                    // for assert
#include <common/CommandBuffers.hpp>  // for CommandBuffers
#include <iostream>                   // for operator<<, endl, basic_ostream
#include <vector>                     // for vector
namespace vkl { class CommandPool; }
namespace vkl { class DescriptorSets; }
namespace vkl { class Device; }
namespace vkl { class GraphicsPipeline; }
namespace vkl { class IBuffer; }
namespace vkl { class RenderPass; }
namespace vkl { class SwapChain; }
// clang-format on

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
