// clang-format off
#include <particle/Compute/ComputeDescriptorSets.hpp>
#include <stddef.h>                           // for size_t
#include <common/VulkanHeader.hpp>               // for VkWriteDescriptorSet
#include <common/misc/DescriptorSet.hpp>      // for writeDescriptorSet
#include <common/struct/ComputeParticle.hpp>  // for ComputeParticle
#include <common/struct/Particle.hpp>         // for Particle
#include <common/Device.hpp>                  // for Device
#include <common/QueueFamily.hpp>             // for vkl
#include <common/buffer/IBuffer.hpp>          // for IBuffer, IUniformBuffers
// clang-format on

using namespace vkl;

void ComputeDescriptorSets::createDescriptorSets() {
  allocateDescriptorSets();

  std::vector<VkWriteDescriptorSet> writeDescriptorSets;

  const VkDescriptorBufferInfo psInfo            = m_buffers[0]->descriptor();
  const VkDescriptorBufferInfo gridInfo          = m_buffers[1]->descriptor();
  const VkDescriptorBufferInfo fsInfo            = m_buffers[2]->descriptor();

  // On paramètre les descripteurs (on se rappelle que l'on en a mit un par frame)
  const IUniformBuffers* ubo = m_uniformBuffers[0];
  for (size_t i = 0; i < m_descriptorSets.size(); i++) {
    const VkDescriptorBufferInfo bufferInfo = ubo->descriptor(i);

    writeDescriptorSets = {
        // Binding 0 :
        misc::writeDescriptorSet(m_descriptorSets.at(i), VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 0, &psInfo),
        misc::writeDescriptorSet(m_descriptorSets.at(i), VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2, &gridInfo),
        misc::writeDescriptorSet(m_descriptorSets.at(i), VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3, &fsInfo),
        // Binding 1 :
        misc::writeDescriptorSet(m_descriptorSets.at(i), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, &bufferInfo),
    };

    vkUpdateDescriptorSets(m_device.logical(), static_cast<uint32_t>(writeDescriptorSets.size()),
                           writeDescriptorSets.data(), 0, nullptr);
  }
}
