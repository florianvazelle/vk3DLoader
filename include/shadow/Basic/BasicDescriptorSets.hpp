#ifndef BASICDESCRIPTORSETS_HPP
#define BASICDESCRIPTORSETS_HPP

#include <common/DescriptorSets.hpp>

namespace vkl {

  class BasicDescriptorSets : public DescriptorSets {
  public:
    BasicDescriptorSets(const Device& device,
                        const SwapChain& swapChain,
                        const DepthRenderPass& renderPass,
                        const UniformBuffers<MVP>& uniformBuffers,
                        const MaterialBuffer& materialUniformBuffer,
                        const UniformBuffers<Depth>& depthUniformBuffer,
                        const DescriptorSetLayout& descriptorSetLayout,
                        const DescriptorPool& descriptorPool);

  private:
    void createDescriptorSets();
  };
}  // namespace vkl

#endif  // BASICDESCRIPTORSETS_HPP