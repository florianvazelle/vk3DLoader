#include <vkl/buffer/Buffer.hpp>

#include <stdexcept>

using namespace vkl;

Buffer::Buffer(const Device& device) : m_device(device) {}

Buffer::~Buffer() {
  // ne dépend pas de la swap chain
  vkDestroyBuffer(m_device.logical(), m_buffer, nullptr);
  vkFreeMemory(m_device.logical(), m_bufferMemory, nullptr);
}

void Buffer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
  // Step 1 - Création du m_buffer
  const VkBufferCreateInfo bufferInfo = {
      .sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .size        = size,
      .usage       = usage,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
  };

  if (vkCreateBuffer(m_device.logical(), &bufferInfo, nullptr, &m_buffer) != VK_SUCCESS) {
    throw std::runtime_error("echec de la creation d'un m_buffer!");
  }

  // Step 2 - Allocation de la mémoire
  // On récupére les fonctionnalités dont le m_buffer a besoin
  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(m_device.logical(), m_buffer, &memRequirements);

  // On rempli la structure VkMemoryAllocateInfo
  const VkMemoryAllocateInfo allocInfo = {
      .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .allocationSize  = memRequirements.size,
      .memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties),
  };

  // Si l'allocation a réussi, nous pouvons associer cette mémoire au m_buffer
  if (vkAllocateMemory(m_device.logical(), &allocInfo, nullptr, &m_bufferMemory) != VK_SUCCESS) {
    throw std::runtime_error("echec de l'allocation de memoire!");
  }

  // Le quatrième paramètre indique le décalage entre le début de la mémoire et le début du m_buffer
  vkBindBufferMemory(m_device.logical(), m_buffer, m_bufferMemory, 0);

  // Note : Pour allouer un grand espace mémoire, il faut que ce nombre soit divisible par
  // memRequirements.alignement
}

uint32_t Buffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
  // On récupère les différents type de mémoire que la carte graphique peut offir
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(m_device.physical(), &memProperties);

  // La structure VkPhysicalDeviceMemoryProperties comprend deux tableaux appelés memoryHeaps et
  // memoryTypes

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    // On itére sur les bits de typeFilter pour trouver les types de mémoire qui lui correspondent
    if ((typeFilter & (1 << i))
        // On vérifie que la mémoire est accesible, properyFlags doit comprend au moins
        // VK_MEMORY_PROPERTY_HOSY_VISIBLE_BIT et VK_MEMORY_PROPERTY_HOSY_COHERENT_BIT
        && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
      return i;
    }
  }

  throw std::runtime_error("aucun type de memoire ne satisfait le buffer!");
}