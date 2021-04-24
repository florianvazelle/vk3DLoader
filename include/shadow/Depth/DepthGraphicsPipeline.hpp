
#ifndef DEPTHGRAPHICSPIPELINE_HPP
#define DEPTHGRAPHICSPIPELINE_HPP

#include <common/GraphicsPipeline.hpp>

namespace vkl {

  class DepthGraphicsPipeline : public GraphicsPipeline {
  public:
    DepthGraphicsPipeline(const Device& device,
                          const SwapChain& swapChain,
                          const RenderPass& renderPass,
                          const DescriptorSetLayout& descriptorSetLayout);

  private:
    void createPipeline();
  };
}  // namespace vkl

#endif  // DEPTHGRAPHICSPIPELINE_HPP