#ifndef DEPTHDESCRIPTORSETS_HPP
#define DEPTHDESCRIPTORSETS_HPP

#include <vkl/DescriptorSets.hpp>

namespace vkl {

  class DepthDescriptorSets : public DescriptorSets {
  public:
    DepthDescriptorSets(const Device& device,
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

#endif  // DEPTHDESCRIPTORSETS_HPP