#include <common/DescriptorSets.hpp>

#include <common/misc/DescriptorSet.hpp>
#include <common/struct/Material.hpp>

using namespace vkl;

DescriptorSets::DescriptorSets(const Device& device,
                               const SwapChain& swapChain,
                               const DepthRenderPass& renderPass,
                               const UniformBuffers<MVP>& uniformBuffers,
                               const MaterialBuffer& materialUniformBuffer,
                               const UniformBuffers<Depth>& depthUniformBuffer,
                               const DescriptorSetLayout& descriptorSetLayout,
                               const DescriptorPool& descriptorPool)
    : m_device(device),
      m_swapChain(swapChain),
      m_renderPass(renderPass),
      m_uniformBuffers(uniformBuffers),
      m_materialUniformBuffer(materialUniformBuffer),
      m_depthUniformBuffer(depthUniformBuffer),
      m_descriptorSetLayout(descriptorSetLayout),
      m_descriptorPool(descriptorPool) {}

void DescriptorSets::recreate() { createDescriptorSets(); }

void DescriptorSets::allocateDescriptorSets() {
  /* Allocate */

  const size_t size = m_swapChain.numImages();

  const std::vector<VkDescriptorSetLayout> layouts(size, m_descriptorSetLayout.handle());
  const VkDescriptorSetAllocateInfo allocInfo
      = misc::descriptorSetAllocateInfo(m_descriptorPool.handle(), layouts.data(), static_cast<uint32_t>(size));

  m_descriptorSets.resize(size);
  if (vkAllocateDescriptorSets(m_device.logical(), &allocInfo, m_descriptorSets.data()) != VK_SUCCESS) {
    throw std::runtime_error("echec de l'allocation d'un set de descripteurs!");
  }
}