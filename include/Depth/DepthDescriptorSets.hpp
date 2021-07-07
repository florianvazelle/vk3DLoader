#ifndef DEPTHDESCRIPTORSETS_HPP
#define DEPTHDESCRIPTORSETS_HPP

#include <vector>        
#include <poike/poike.hpp>

using namespace poike;

namespace vkl {

  class DepthDescriptorSets : public DescriptorSets {
  public:
    DepthDescriptorSets(const Device& device,
                        const SwapChain& swapChain,
                        const DescriptorSetLayout& descriptorSetLayout,
                        const DescriptorPool& descriptorPool,
                        const std::vector<const IBuffer*>& buffers,
                        const std::vector<const IUniformBuffers*>& uniformBuffers)
        : DescriptorSets(device,
                         swapChain,
                         descriptorSetLayout,
                         descriptorPool,
                         buffers,
                         uniformBuffers) {
      createDescriptorSets();
    }

  private:
    void createDescriptorSets() final;
  };
}  // namespace vkl

#endif  // DEPTHDESCRIPTORSETS_HPP
