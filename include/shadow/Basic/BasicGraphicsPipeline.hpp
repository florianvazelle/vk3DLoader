
#ifndef BASICGRAPHICSPIPELINE_HPP
#define BASICGRAPHICSPIPELINE_HPP

#include <common/GraphicsPipeline.hpp>

namespace vkl {

  class BasicGraphicsPipeline : public GraphicsPipeline {
  public:
    BasicGraphicsPipeline(const Device& device,
                          const SwapChain& swapChain,
                          const RenderPass& renderPass,
                          const DescriptorSetLayout& descriptorSetLayout);

  private:
    void createPipeline();
  };
}  // namespace vkl

#endif  // BASICGRAPHICSPIPELINE_HPP