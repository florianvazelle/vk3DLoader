#ifndef DESCRIPTORSETS_HPP
#define DESCRIPTORSETS_HPP

#include <vulkan/vulkan.h>

#include <NonCopyable.hpp>
#include <vkl/DescriptorPool.hpp>
#include <vkl/DescriptorSetLayout.hpp>
#include <vkl/Device.hpp>
#include <vkl/buffer/UniformBuffers.hpp>

namespace vkl {

  class DescriptorSets : public NonCopyable {
  public:
    DescriptorSets(const Device& device,
                   const SwapChain& swapChain,
                   const UniformBuffers& uniformBuffers,
                   const DescriptorSetLayout& descriptorSetLayout,
                   const DescriptorPool& descriptorPool);
    // ~DescriptorSets(); no need destructor because VkDescriptorSet is deleted when pool is deleted

    inline const VkDescriptorSet& descriptor(int index) const { return m_descriptorSets.at(index); }

    void recreate();

  private:
    std::vector<VkDescriptorSet> m_descriptorSets;

    const Device& m_device;
    const SwapChain& m_swapChain;
    const UniformBuffers& m_uniformBuffers;
    const DescriptorSetLayout& m_descriptorSetLayout;
    const DescriptorPool& m_descriptorPool;

    void createDescriptorSets();
  };
}  // namespace vkl

#endif  // DESCRIPTORSETLAYOUT_HPP