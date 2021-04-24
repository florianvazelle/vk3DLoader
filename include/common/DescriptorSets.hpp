/**
 * @file DescriptorSets.hpp
 * @brief Define DescriptorSets base class
 */

#ifndef DESCRIPTORSETS_HPP
#define DESCRIPTORSETS_HPP

#include <vulkan/vulkan.h>

#include <NonCopyable.hpp>
#include <common/DescriptorPool.hpp>
#include <common/DescriptorSetLayout.hpp>
#include <common/Device.hpp>
#include <common/buffer/Buffer.hpp>
#include <common/buffer/UniformBuffers.hpp>
#include <common/struct/Depth.hpp>
#include <common/struct/MVP.hpp>
#include <shadow/Basic/BasicRenderPass.hpp>
#include <shadow/Depth/DepthRenderPass.hpp>

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