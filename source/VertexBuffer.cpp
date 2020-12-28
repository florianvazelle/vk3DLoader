#include <vkl/VertexBuffer.hpp>

#include <cstring>
#include <vkl/Utils.hpp>

using namespace vkl;

VertexBuffer::VertexBuffer(const Device& device, const std::vector<Vertex>& vertices)
    : m_device(device), m_vertices(vertices) {
  // Step 1 - Création du buffer
  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = sizeof(m_vertices[0]) * m_vertices.size();
  bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(m_device.logical(), &bufferInfo, nullptr, &m_vertexBuffer) != VK_SUCCESS) {
    throw std::runtime_error("Echec de la creation d'un vertex buffer!");
  }

  // Step 2 - Allocation de la mémoire
  // On récupére les fonctionnalités dont le buffer a besoin
  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(m_device.logical(), m_vertexBuffer, &memRequirements);

  // On rempli la structure VkMemoryAllocateInfo
  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex
      = findMemoryType(m_device, memRequirements.memoryTypeBits,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  // Si l'allocation a réussi, nous pouvons associer cette mémoire au buffer
  if (vkAllocateMemory(m_device.logical(), &allocInfo, nullptr, &m_vertexBufferMemory)
      != VK_SUCCESS) {
    throw std::runtime_error("echec d'une allocation de memoire!");
  }

  // Le quatrième paramètre indique le décalage entre le début de la mémoire et le début du buffer
  vkBindBufferMemory(m_device.logical(), m_vertexBuffer, m_vertexBufferMemory, 0);

  // Note : Pour allouer un grand espace mémoire, il faut que ce nombre soit divisible par
  // memRequirements.alignement

  // Step 3 - Remplissage du vertex buffer
  void* data;

  /**
   * Note Exposé : La fonction vkMapMemory permet d'accéder à une région spécifique d'une ressource.
   * Il faut indiquer un décalage et une taille. On met ici 0 et bufferInfo.size.
   * L'avant-dernier paramètre est un champ de bits (flags) pour l'instant non implémenté par
   * Vulkan. Il est impératif de le laisser à 0. "is reserved for future use"
   * Le dernier paramètre permet de fournir un pointeur vers la mémoire mappée.
   */
  vkMapMemory(m_device.logical(), m_vertexBufferMemory, 0, bufferInfo.size, 0, &data);
  memcpy(data, m_vertices.data(), (size_t)bufferInfo.size);
  vkUnmapMemory(m_device.logical(), m_vertexBufferMemory);
}

VertexBuffer::~VertexBuffer() {
  // ne dépend pas de la swap chain
  vkDestroyBuffer(m_device.logical(), m_vertexBuffer, nullptr);
  vkFreeMemory(m_device.logical(), m_vertexBufferMemory, nullptr);
}
