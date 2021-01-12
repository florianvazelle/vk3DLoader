#ifndef DESCRIPTORSETS_HPP
#define DESCRIPTORSETS_HPP

#include <vulkan/vulkan.h>

#include <NonCopyable.hpp>
#include <vkl/Basic/BasicRenderPass.hpp>
#include <vkl/Depth/DepthRenderPass.hpp>
#include <vkl/DescriptorPool.hpp>
#include <vkl/DescriptorSetLayout.hpp>
#include <vkl/Device.hpp>
#include <vkl/buffer/Buffer.hpp>
#include <vkl/buffer/UniformBuffers.hpp>
#include <vkl/struct/Depth.hpp>
#include <vkl/struct/MVP.hpp>

namespace vkl {

  class DescriptorSets : public NonCopyable {
  public:
    DescriptorSets(const Device& device,
                   const SwapChain& swapChain,
                   const DepthRenderPass& renderPass,
                   const UniformBuffers<MVP>& uniformBuffers,
                   const MaterialBuffer& materialUniformBuffer,
                   const UniformBuffers<Depth>& depthUniformBuffer,
                   const DescriptorSetLayout& descriptorSetLayout,
                   const DescriptorPool& descriptorPool);
    // ~DescriptorSets(); no need destructor because VkDescriptorSet is deleted when pool is deleted

    inline const VkDescriptorSet& descriptor(int index) const { return m_descriptorSets.at(index); }

    void recreate();

  protected:
    std::vector<VkDescriptorSet> m_descriptorSets;

    const Device& m_device;
    const SwapChain& m_swapChain;
    const DepthRenderPass& m_renderPass;
    const UniformBuffers<MVP>& m_uniformBuffers;
    const MaterialBuffer& m_materialUniformBuffer;
    const UniformBuffers<Depth>& m_depthUniformBuffer;
    const DescriptorSetLayout& m_descriptorSetLayout;
    const DescriptorPool& m_descriptorPool;

    void allocateDescriptorSets();
    virtual void createDescriptorSets() = 0;
  };
}  // namespace vkl

#endif  // DESCRIPTORSETLAYOUT_HPP