#include <vkl/DescriptorSets.hpp>

#include <vkl/UniformBufferObject.hpp>

using namespace vkl;

DescriptorSets::DescriptorSets(const Device& device,
                               const SwapChain& swapChain,
                               const UniformBuffers& uniformBuffers,
                               const DescriptorSetLayout& descriptorSetLayout,
                               const DescriptorPool& descriptorPool)
    : m_device(device),
      m_swapChain(swapChain),
      m_uniformBuffers(uniformBuffers),
      m_descriptorSetLayout(descriptorSetLayout),
      m_descriptorPool(descriptorPool) {
  createDescriptorSets();
}

void DescriptorSets::recreate() { createDescriptorSets(); }

void DescriptorSets::createDescriptorSets() {
  const size_t size = m_swapChain.numImages();

  const std::vector<VkDescriptorSetLayout> layouts(size, m_descriptorSetLayout.handle());
  const VkDescriptorSetAllocateInfo allocInfo = {
      .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
      .descriptorPool     = m_descriptorPool.handle(),
      .descriptorSetCount = static_cast<uint32_t>(size),
      .pSetLayouts        = layouts.data(),
  };

  m_descriptorSets.resize(size);
  if (vkAllocateDescriptorSets(m_device.logical(), &allocInfo, m_descriptorSets.data()) != VK_SUCCESS) {
    throw std::runtime_error("echec de l'allocation d'un set de descripteurs!");
  }

  // On paramètre les descripteurs (on se rappelle que l'on en a mit un par frame)
  for (size_t i = 0; i < size; i++) {
    const VkDescriptorBufferInfo bufferInfo = {
        .buffer = m_uniformBuffers.buffer(i),
        .offset = 0,
        .range  = sizeof(UniformBufferObject),
    };

    const VkWriteDescriptorSet descriptorWrite = {
        .sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet          = m_descriptorSets[i],  // set à mettre à jour
        .dstBinding      = 0,                    // indice du binding
        .dstArrayElement = 0,                    // indice du premier élément du tableau à modifier
        .descriptorCount = 1,
        .descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo     = &bufferInfo,
    };

    vkUpdateDescriptorSets(m_device.logical(), 1, &descriptorWrite, 0, nullptr);
  }
}