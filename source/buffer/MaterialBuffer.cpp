#include <vkl/buffer/MaterialBuffer.hpp>

#include <cstring>

using namespace vkl;

MaterialBuffer::MaterialBuffer(const Device& device, const Material& material) : Buffer(device), m_material(material) {
  const VkDeviceSize bufferSize = sizeof(Material);

  createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  void* data;
  vkMapMemory(m_device.logical(), m_bufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, &material, (size_t)bufferSize);
  vkUnmapMemory(m_device.logical(), m_bufferMemory);
}