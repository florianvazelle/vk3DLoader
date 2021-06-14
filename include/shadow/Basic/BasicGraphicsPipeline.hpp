#ifndef BASICGRAPHICSPIPELINE_HPP
#define BASICGRAPHICSPIPELINE_HPP

// clang-format off
#include <common/GraphicsPipeline.hpp>  // for GraphicsPipeline
namespace vkl { class DescriptorSetLayout; }
namespace vkl { class Device; }
namespace vkl { class RenderPass; }
namespace vkl { class SwapChain; }
// clang-format on

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