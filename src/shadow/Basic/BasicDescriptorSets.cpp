#include <shadow/Basic/BasicDescriptorSets.hpp>

#include <common/misc/DescriptorSet.hpp>
#include <common/struct/Material.hpp>

using namespace vkl;

BasicDescriptorSets::BasicDescriptorSets(const Device& device,
                                         const SwapChain& swapChain,
                                         const DepthRenderPass& renderPass,
                                         const UniformBuffers<MVP>& uniformBuffers,
                                         const MaterialBuffer& materialUniformBuffer,
                                         const UniformBuffers<Depth>& depthUniformBuffer,
                                         const DescriptorSetLayout& descriptorSetLayout,
                                         const DescriptorPool& descriptorPool)
    : DescriptorSets(device,
                     swapChain,
                     renderPass,
                     uniformBuffers,
                     materialUniformBuffer,
                     depthUniformBuffer,
                     descriptorSetLayout,
                     descriptorPool) {
  createDescriptorSets();
}

/**
 * Note Exposé : Creation d'un descriptor set
 * 1. Allocation
 * 2. Update avec un Buffer
 */
void BasicDescriptorSets::createDescriptorSets() {
  allocateDescriptorSets();

  /* Update */

  std::vector<VkWriteDescriptorSet> writeDescriptorSets;

  const VkDescriptorBufferInfo materialBufferInfo = {
      .buffer = m_materialUniformBuffer.buffer(),
      .offset = 0,
      .range  = sizeof(Material),
  };

  // On paramètre les descripteurs (on se rappelle que l'on en a mit un par frame)
  for (size_t i = 0; i < m_descriptorSets.size(); i++) {
    const VkDescriptorBufferInfo bufferInfo = {
        .buffer = m_uniformBuffers.buffer(i),
        .offset = 0,
        .range  = sizeof(MVP),
    };

    // Image descriptor for the shadow map attachment
    const VkDescriptorImageInfo depthDescriptor = {
        // le shader va lire les valeurs écrits a cette position, juste avant
        .sampler     = m_renderPass.depthAttachment(i).sample(),
        .imageView   = m_renderPass.depthAttachment(i).view(),
        .imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
    };

    writeDescriptorSets = {
        // Binding 0 : Vertex shader uniform buffer
        misc::writeDescriptorSet(m_descriptorSets.at(i), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, &bufferInfo),
        // Binding 1 : Fragment shader Depth input attachment
        misc::writeDescriptorSet(m_descriptorSets.at(i), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1,
                                 &depthDescriptor),
        // Binding 2 : Fragment shader uniform buffer
        misc::writeDescriptorSet(m_descriptorSets.at(i), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2, &materialBufferInfo),
    };

    vkUpdateDescriptorSets(m_device.logical(), writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);
  }
}