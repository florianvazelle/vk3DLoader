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
#include <common/CommandPool.hpp>
#include <common/image/Texture.hpp>
#include <common/image/Attachment.hpp>
// clang-format on

namespace vkl {

  class BasicDescriptorSets : public DescriptorSets {
  public:
    BasicDescriptorSets(const Device& device,
                        const SwapChain& swapChain,
                        const DescriptorSetLayout& descriptorSetLayout,
                        const DescriptorPool& descriptorPool,
                        const std::vector<const IBuffer*>& buffers,
                        const std::vector<const IUniformBuffers*>& uniformBuffers,
                        const std::vector<std::unique_ptr<Texture>>& textures,
                        const std::vector<std::unique_ptr<Attachment>>& attachments)
        : DescriptorSets(device,
                         swapChain,
                         descriptorSetLayout,
                         descriptorPool,
                         buffers,
                         uniformBuffers),
          m_textures(textures),
          m_attachments(attachments) {
      createDescriptorSets();
    }

  private:
    const std::vector<std::unique_ptr<Texture>>& m_textures;
    const std::vector<std::unique_ptr<Attachment>>& m_attachments;

    void createDescriptorSets() final;
  };
}  // namespace vkl

#endif  // BASICDESCRIPTORSETS_HPP
