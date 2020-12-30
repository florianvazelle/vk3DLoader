#include <vkl/DescriptorPool.hpp>

#include <stdexcept>

using namespace vkl;

DescriptorPool::DescriptorPool(const Device& device, const SwapChain& swapChain)
    : m_device(device), m_swapChain(swapChain) {
  createDescriptorPool();
}

DescriptorPool::~DescriptorPool() { cleanup(); }

void DescriptorPool::cleanup() { vkDestroyDescriptorPool(m_device.logical(), m_pool, nullptr); }

void DescriptorPool::recreate() {
  cleanup();
  createDescriptorPool();
}

void DescriptorPool::createDescriptorPool() {
  const VkDescriptorPoolSize poolSize = {
      // indique les types de descripteurs
      .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      // et combien sont compris dans les sets (de descripteurs)
      .descriptorCount = static_cast<uint32_t>(m_swapChain.numImages()),
  };

  // un descripteur par frame
  const VkDescriptorPoolCreateInfo poolInfo = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
      // nombre maximum de sets à allouer
      .maxSets       = static_cast<uint32_t>(m_swapChain.numImages()),
      .poolSizeCount = 1,
      .pPoolSizes    = &poolSize,
  };

  if (vkCreateDescriptorPool(m_device.logical(), &poolInfo, nullptr, &m_pool) != VK_SUCCESS) {
    throw std::runtime_error("echec de la creation de la pool de descripteurs!");
  }
}