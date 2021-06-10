// clang-format off
#include <shadow/Depth/DepthDescriptorSets.hpp>
#include <stddef.h>                       // for size_t
#include <vulkan/vulkan_core.h>           // for VkWriteDescriptorSet, vkUpd...
#include <common/misc/DescriptorSet.hpp>  // for writeDescriptorSet
#include <common/struct/Depth.hpp>        // for Depth
#include <common/Device.hpp>              // for Device
#include <common/QueueFamily.hpp>         // for vkl
#include <common/buffer/IBuffer.hpp>      // for IUniformBuffers
// clang-format on

using namespace vkl;

void DepthDescriptorSets::createDescriptorSets() {
  allocateDescriptorSets();

  /* Update */
  std::vector<VkWriteDescriptorSet> writeDescriptorSets;
  const IUniformBuffers* depthUBO = m_uniformBuffers[0];

  for (size_t i = 0; i < m_descriptorSets.size(); i++) {
    const VkDescriptorBufferInfo bufferInfo = {
        .buffer = depthUBO->buffer(i),
        .offset = 0,
        .range  = sizeof(Depth),
    };

    writeDescriptorSets = {
        misc::writeDescriptorSet(m_descriptorSets.at(i), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, &bufferInfo),
    };

    vkUpdateDescriptorSets(m_device.logical(), writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);
  }
}
