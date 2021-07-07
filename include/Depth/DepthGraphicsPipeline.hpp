#ifndef DEPTHGRAPHICSPIPELINE_HPP
#define DEPTHGRAPHICSPIPELINE_HPP

#include <poike/poike.hpp>

using namespace poike;

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