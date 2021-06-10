#ifndef DEPTHDESCRIPTORSETS_HPP
#define DEPTHDESCRIPTORSETS_HPP

// clang-format off
#include <common/DescriptorSets.hpp>  // for DescriptorSets
#include <vector>                     // for vector
namespace vkl { class DescriptorPool; }
namespace vkl { class DescriptorSetLayout; }
namespace vkl { class Device; }
namespace vkl { class IBuffer; }
namespace vkl { class IRenderPass; }
namespace vkl { class IUniformBuffers; }
namespace vkl { class SwapChain; }
// clang-format on

namespace vkl {

  class DepthDescriptorSets : public DescriptorSets {
  public:
    DepthDescriptorSets(const Device& device,
                        const SwapChain& swapChain,
                        const DescriptorSetLayout& descriptorSetLayout,
                        const DescriptorPool& descriptorPool,
                        const std::vector<const IRenderPass*>& renderPasses,
                        const std::vector<const IBuffer*>& buffers,
                        const std::vector<const IUniformBuffers*>& uniformBuffers)
        : DescriptorSets(device,
                         swapChain,
                         descriptorSetLayout,
                         descriptorPool,
                         renderPasses,
                         buffers,
                         uniformBuffers) {
      createDescriptorSets();
    }

  private:
    void createDescriptorSets();
  };
}  // namespace vkl

#endif  // DEPTHDESCRIPTORSETS_HPP
