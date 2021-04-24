#include <shadow/Depth/DepthDescriptorSets.hpp>

#include <common/misc/DescriptorSet.hpp>
#include <common/struct/Material.hpp>

using namespace vkl;

DepthDescriptorSets::DepthDescriptorSets(const Device& device,
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

void DepthDescriptorSets::createDescriptorSets() {
  allocateDescriptorSets();

  /* Update */
  std::vector<VkWriteDescriptorSet> writeDescriptorSets;

  for (size_t i = 0; i < m_descriptorSets.size(); i++) {
    const VkDescriptorBufferInfo bufferInfo = {
        .buffer = m_depthUniformBuffer.buffer(i),
        .offset = 0,
        .range  = sizeof(Depth),
    };

    writeDescriptorSets = {
        misc::writeDescriptorSet(m_descriptorSets.at(i), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, &bufferInfo),
    };

    vkUpdateDescriptorSets(m_device.logical(), writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);
  }
}