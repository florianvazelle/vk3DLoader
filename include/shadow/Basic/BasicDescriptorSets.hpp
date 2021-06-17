#ifndef BASICDESCRIPTORSETS_HPP
#define BASICDESCRIPTORSETS_HPP

// clang-format off
#include <common/DescriptorSets.hpp>  // for DescriptorSets
#include <vector>                     // for vector
namespace vkl { class DescriptorPool; }
namespace vkl { class DescriptorSetLayout; }
namespace vkl { class Device; }
namespace vkl { class IBuffer; }
namespace vkl { class RenderPass; }
namespace vkl { class IUniformBuffers; }
namespace vkl { class SwapChain; }
// clang-format on

namespace vkl {

  class BasicDescriptorSets : public DescriptorSets {
  public:
    BasicDescriptorSets(const Device& device,
                        const SwapChain& swapChain,
                        const DescriptorSetLayout& descriptorSetLayout,
                        const DescriptorPool& descriptorPool,
                        const std::vector<const RenderPass*>& renderPasses,
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
    void createDescriptorSets() final;
  };
}  // namespace vkl

#endif  // BASICDESCRIPTORSETS_HPP