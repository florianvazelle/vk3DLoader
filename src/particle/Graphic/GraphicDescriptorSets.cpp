// clang-format off
#include <particle/Graphic/GraphicDescriptorSets.hpp>
#include <stddef.h>                       // for size_t
#include <vulkan/vulkan_core.h>           // for VkWriteDescriptorSet, vkUpd...
#include <common/misc/DescriptorSet.hpp>  // for writeDescriptorSet
#include <common/struct/MVP.hpp>          // for MVP
#include <common/Device.hpp>              // for Device
#include <common/QueueFamily.hpp>         // for vkl
#include <common/buffer/IBuffer.hpp>      // for IUniformBuffers
// clang-format on

using namespace vkl;

void GraphicDescriptorSets::createDescriptorSets() {
  allocateDescriptorSets();

  std::vector<VkWriteDescriptorSet> writeDescriptorSets;

  // On paramètre les descripteurs (on se rappelle que l'on en a mit un par frame)
  const IUniformBuffers* ubo = m_uniformBuffers[0];
  for (size_t i = 0; i < m_descriptorSets.size(); i++) {
    const VkDescriptorBufferInfo bufferInfo = {
        .buffer = ubo->buffer(i),
        .offset = 0,
        .range  = sizeof(MVP),
    };

    writeDescriptorSets = {
        // Binding 0 :
        // misc::writeDescriptorSet(m_descriptorSets.at(i), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, &bufferInfo),
        // Binding 1 :
        // misc::writeDescriptorSet(m_descriptorSets.at(i), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1,
        // &depthDescriptor),
        // Binding 2 :
        misc::writeDescriptorSet(m_descriptorSets.at(i), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2, &bufferInfo),
    };

    vkUpdateDescriptorSets(m_device.logical(), writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);
  }
}