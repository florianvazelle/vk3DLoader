#ifndef DEPTHGRAPHICSPIPELINE_HPP
#define DEPTHGRAPHICSPIPELINE_HPP

// clang-format off
#include <common/GraphicsPipeline.hpp>  // for GraphicsPipeline
namespace vkl { class DescriptorSetLayout; }
namespace vkl { class Device; }
namespace vkl { class RenderPass; }
namespace vkl { class SwapChain; }
// clang-format on

namespace vkl {

  class DepthGraphicsPipeline : public GraphicsPipeline {
  public:
    DepthGraphicsPipeline(const Device& device,
                          const SwapChain& swapChain,
                          const RenderPass& renderPass,
                          const DescriptorSetLayout& descriptorSetLayout);
    ~DepthGraphicsPipeline();

  private:
    void createPipeline() final;
  };
}  // namespace vkl

#endif  // DEPTHGRAPHICSPIPELINE_HPP