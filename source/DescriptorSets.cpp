#include <vkl/DescriptorSets.hpp>

#include <vkl/misc/DescriptorSet.hpp>
#include <vkl/struct/Material.hpp>

using namespace vkl;

DescriptorSets::DescriptorSets(const Device& device,
                               const SwapChain& swapChain,
                               const UniformBuffers<MVP>& uniformBuffers,
                               const MaterialBuffer& materialUniformBuffer,
                               const UniformBuffers<Depth>& depthUniformBuffer,
                               const DescriptorSetLayout& descriptorSetLayout,
                               const DescriptorPool& descriptorPool,
                               const DepthRenderPass& depthRenderPass)
    : m_device(device),
      m_swapChain(swapChain),
      m_uniformBuffers(uniformBuffers),
      m_materialUniformBuffer(materialUniformBuffer),
      m_depthUniformBuffer(depthUniformBuffer),
      m_descriptorSetLayout(descriptorSetLayout),
      m_descriptorPool(descriptorPool),
      m_depthRenderPass(depthRenderPass) {
  createDescriptorSets();
}

void DescriptorSets::recreate() { createDescriptorSets(); }

/**
 * Note Exposé : Creation d'un descriptor set
 * 1. Allocation
 * 2. Update avec un Buffer
 */
void DescriptorSets::createDescriptorSets() {
  // Image descriptor for the shadow map attachment
  VkDescriptorImageInfo shadowMapDescriptor = {
      .sampler     = m_depthRenderPass.sample(),
      .imageView   = m_depthRenderPass.view(),
      .imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
  };

  /* Allocate */

  const size_t size = 1;  // m_swapChain.numImages();

  const std::vector<VkDescriptorSetLayout> layouts(size, m_descriptorSetLayout.handle());
  const VkDescriptorSetAllocateInfo allocInfo
      = misc::descriptorSetAllocateInfo(m_descriptorPool.handle(), layouts.data(), static_cast<uint32_t>(size));

  m_descriptorSets.resize(size);
  if (vkAllocateDescriptorSets(m_device.logical(), &allocInfo, m_descriptorSets.data()) != VK_SUCCESS) {
    throw std::runtime_error("echec de l'allocation d'un set de descripteurs!");
  }

  /* Update */

  std::vector<VkWriteDescriptorSet> writeDescriptorSets;

  const VkDescriptorBufferInfo materialBufferInfo = {
      .buffer = m_materialUniformBuffer.buffer(),
      .offset = 0,
      .range  = sizeof(Material),
  };

  // On paramètre les descripteurs (on se rappelle que l'on en a mit un par frame)
  for (size_t i = 0; i < size; i++) {
    const VkDescriptorBufferInfo bufferInfo = {
        .buffer = m_uniformBuffers.buffer(i),
        .offset = 0,
        .range  = sizeof(MVP),
    };

    writeDescriptorSets = {
        // Binding 0 : Vertex shader uniform buffer
        misc::writeDescriptorSet(m_descriptorSets.at(i), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, &bufferInfo),
        // Binding 1 : Fragment shader shadow sampler
        misc::writeDescriptorSet(m_descriptorSets.at(i), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1,
                                 &shadowMapDescriptor),
        // Binding 2 : Fragment shader uniform buffer
        misc::writeDescriptorSet(m_descriptorSets.at(i), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2, &materialBufferInfo),
    };

    vkUpdateDescriptorSets(m_device.logical(), writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);
  }

  /* Depth Descriptor Set */
  // TODO : Separate this in a new Descriprot set

  // Offscreen shadow map generation
  m_dethDescriptorSet.resize(size);
  if (vkAllocateDescriptorSets(m_device.logical(), &allocInfo, m_dethDescriptorSet.data()) != VK_SUCCESS) {
    throw std::runtime_error("echec de l'allocation d'un set de descripteurs!");
  }

  for (size_t i = 0; i < size; i++) {
    const VkDescriptorBufferInfo bufferInfo = {
        .buffer = m_depthUniformBuffer.buffer(i),
        .offset = 0,
        .range  = sizeof(Depth),
    };

    writeDescriptorSets = {
        misc::writeDescriptorSet(m_dethDescriptorSet.at(i), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, &bufferInfo),
    };

    vkUpdateDescriptorSets(m_device.logical(), writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);
  }
}