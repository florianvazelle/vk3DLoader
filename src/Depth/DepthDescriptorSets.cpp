#include <Depth/DepthDescriptorSets.hpp>

#include <poike/poike.hpp>

using namespace poike;
using namespace vkl;

void DepthDescriptorSets::createDescriptorSets() {
  allocateDescriptorSets();

  /* Update */
  std::vector<VkWriteDescriptorSet> writeDescriptorSets;
  const IUniformBuffers* depthUBO = m_uniformBuffers[0];

  for (size_t i = 0; i < m_descriptorSets.size(); i++) {
    const VkDescriptorBufferInfo& bufferInfo = depthUBO->descriptor(i);

    writeDescriptorSets = {
        misc::writeDescriptorSet(m_descriptorSets.at(i), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, &bufferInfo),
    };

    vkUpdateDescriptorSets(m_device.logical(), writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);
  }
}
