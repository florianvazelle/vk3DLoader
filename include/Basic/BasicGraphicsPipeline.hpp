#ifndef BASICGRAPHICSPIPELINE_HPP
#define BASICGRAPHICSPIPELINE_HPP

#include <poike/poike.hpp>

using namespace poike;

namespace vkl {

  class BasicGraphicsPipeline : public GraphicsPipeline {
  public:
    BasicGraphicsPipeline(const Device& device,
                          const SwapChain& swapChain,
                          const RenderPass& renderPass,
                          const DescriptorSetLayout& descriptorSetLayout);
    ~BasicGraphicsPipeline();

  private:
    void createPipeline() final;
  };
}  // namespace vkl

#endif  // BASICGRAPHICSPIPELINE_HPP