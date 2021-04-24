#include <common/CommandPool.hpp>
#include <common/Device.hpp>

#include <stdexcept>

using namespace vkl;

CommandPool::CommandPool(const Device& device, const VkCommandPoolCreateFlags& flags)
    : m_device(device), m_flags(flags) {
  const VkCommandPoolCreateInfo poolInfo = {
      .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .flags            = flags,
      .queueFamilyIndex = m_device.queueFamilyIndices().graphicsFamily.value(),
  };

  if (vkCreateCommandPool(m_device.logical(), &poolInfo, nullptr, &m_pool) != VK_SUCCESS) {
    throw std::runtime_error("failed to create command pool!");
  }
}

CommandPool::~CommandPool() { vkDestroyCommandPool(m_device.logical(), m_pool, nullptr); }