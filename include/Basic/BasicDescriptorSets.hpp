#ifndef BASICDESCRIPTORSETS_HPP
#define BASICDESCRIPTORSETS_HPP

#include <vector>  
#include <poike/poike.hpp>

using namespace poike;

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
