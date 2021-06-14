#ifndef BASICCOMMANDBUFFERS_HPP
#define BASICCOMMANDBUFFERS_HPP

// clang-format off
#include <common/CommandBuffers.hpp>  // for CommandBuffers
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

  class BasicCommandBuffers : public CommandBuffers {
  public:
    BasicCommandBuffers(const Device& device,
                        const RenderPass& renderPass,
                        const SwapChain& swapChain,
                        const GraphicsPipeline& graphicsPipeline,
                        const CommandPool& commandPool,
                        const DescriptorSets& descriptorSets,
                        const std::vector<const IBuffer*>& buffers)
        : CommandBuffers(device, renderPass, swapChain, graphicsPipeline, commandPool, descriptorSets, buffers) {
      createCommandBuffers();
    }

  private:
    void createCommandBuffers() final;
  };

}  // namespace vkl

#endif  // BASICCOMMANDBUFFERS_HPP
