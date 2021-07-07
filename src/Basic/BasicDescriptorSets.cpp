#include <Basic/BasicDescriptorSets.hpp>
#include <stddef.h>                         
#include <poike/poike.hpp>

using namespace poike;
using namespace vkl;

/**
 * Note Exposé : Creation d'un descriptor set
 * 1. Allocation
 * 2. Update avec un Buffer
 */
void BasicDescriptorSets::createDescriptorSets() {
  allocateDescriptorSets();

  /* Update */

  std::vector<VkWriteDescriptorSet> writeDescriptorSets;

  const IBuffer* materialBuffer                    = m_buffers[0];
  const VkDescriptorBufferInfo& materialBufferInfo = materialBuffer->descriptor();

  // Texture Descriptor
  const std::unique_ptr<Texture>& texture = m_textures[0];
  const VkDescriptorImageInfo imageInfo = {
    .sampler = texture->sample(),
    .imageView =  texture->view(),
    .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
  };

  // On paramètre les descripteurs (on se rappelle que l'on en a mit un par frame)
  const IUniformBuffers* ubo   = m_uniformBuffers[0];
  for (size_t i = 0; i < m_descriptorSets.size(); i++) {
    const VkDescriptorBufferInfo& bufferInfo = ubo->descriptor(i);

    // Image descriptor for the shadow map attachment
    const std::unique_ptr<Attachment>& depth_map = m_attachments[i];
    const VkDescriptorImageInfo depthDescriptor = {
        // le shader va lire les valeurs écrits a cette position, juste avant
        .sampler     = depth_map->sample(),
        .imageView   = depth_map->view(),
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

        //Binding 3 :
        misc::writeDescriptorSet(m_descriptorSets.at(i), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3, &imageInfo),
    };

    vkUpdateDescriptorSets(m_device.logical(), writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);
  }
}
