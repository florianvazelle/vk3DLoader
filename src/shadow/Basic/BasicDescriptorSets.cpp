// clang-format off
#include <shadow/Basic/BasicDescriptorSets.hpp>
#include <stddef.h>                          // for size_t
#include <common/VulkanHeader.hpp>              // for VkWriteDescriptorSet
#include <common/misc/DescriptorSet.hpp>     // for writeDescriptorSet
#include <common/struct/DepthMVP.hpp>        // for DepthMVP
#include <common/struct/Material.hpp>        // for Material
#include <common/Device.hpp>                 // for Device
#include <common/FrameBufferAttachment.hpp>  // for FrameBufferAttachment
#include <common/QueueFamily.hpp>            // for vkl
#include <common/RenderPass.hpp>             // for RenderPass
#include <common/buffer/IBuffer.hpp>         // for IBuffer, IUniformBuffers
// clang-format on

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

  // On paramètre les descripteurs (on se rappelle que l'on en a mit un par frame)
  const IUniformBuffers* ubo   = m_uniformBuffers[0];
  const RenderPass* renderPass = m_renderPasses[0];
  for (size_t i = 0; i < m_descriptorSets.size(); i++) {
    const VkDescriptorBufferInfo& bufferInfo = ubo->descriptor(i);

    // Image descriptor for the shadow map attachment
    const VkDescriptorImageInfo depthDescriptor = {
        // le shader va lire les valeurs écrits a cette position, juste avant
        .sampler     = renderPass->depthAttachment(i).sample(),
        .imageView   = renderPass->depthAttachment(i).view(),
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
