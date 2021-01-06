#include <vkl/buffer/VertexBuffer.hpp>

#include <cstring>

using namespace vkl;

VertexBuffer::VertexBuffer(const Device& device, const std::vector<Vertex>& vertices)
    : Buffer(device), m_vertices(vertices) {
  const VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

  // Voir commentaires dans la fonction createBuffer
  createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  // Step 3 - Remplissage du vertex buffer
  void* data;

  /**
   * Note Exposé : La fonction vkMapMemory permet d'accéder à une région spécifique d'une ressource.
   * Il faut indiquer un décalage et une taille. On met ici 0 et bufferInfo.size.
   * L'avant-dernier paramètre est un champ de bits (flags) pour l'instant non implémenté par
   * Vulkan. Il est impératif de le laisser à 0. "is reserved for future use"
   * Le dernier paramètre permet de fournir un pointeur vers la mémoire mappée.
   */
  vkMapMemory(m_device.logical(), m_bufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, m_vertices.data(), (size_t)bufferSize);
  vkUnmapMemory(m_device.logical(), m_bufferMemory);
}