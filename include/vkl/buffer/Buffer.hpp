#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <vulkan/vulkan.h>
#include <cstring>  // memcpy
#include <stdexcept>

#include <NonCopyable.hpp>
#include <vkl/Device.hpp>
#include <vkl/misc/Device.hpp>
#include <vkl/struct/Material.hpp>
#include <vkl/struct/Vertex.hpp>

namespace vkl {

  /**
   * @brief A templating class to define buffer
   */
  template <typename T> class Buffer : public NonCopyable {
  public:
    Buffer(const Device& device, const T& bufferData) : m_device(device), m_bufferData(bufferData) {}
    Buffer(const Device& device, const T& bufferData, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
        : Buffer(device, bufferData) {
      m_bufferSize = sizeof(bufferData);

      // Voir commentaires dans la fonction createBuffer
      createBuffer(m_bufferSize, usage, properties);

      // Step 3 - Remplissage du vertex buffer
      void* data;

      /**
       * Note Exposé : La fonction vkMapMemory permet d'accéder à une région spécifique d'une ressource.
       * Il faut indiquer un décalage et une taille. On met ici 0 et bufferInfo.size.
       * L'avant-dernier paramètre est un champ de bits (flags) pour l'instant non implémenté par
       * Vulkan. Il est impératif de le laisser à 0. "is reserved for future use"
       * Le dernier paramètre permet de fournir un pointeur vers la mémoire mappée.
       */
      vkMapMemory(m_device.logical(), m_bufferMemory, 0, m_bufferSize, 0, &data);
      memcpy(data, &m_bufferData, (size_t)m_bufferSize);
      vkUnmapMemory(m_device.logical(), m_bufferMemory);
    }

    ~Buffer() {
      // ne dépend pas de la swap chain
      vkDestroyBuffer(m_device.logical(), m_buffer, nullptr);
      vkFreeMemory(m_device.logical(), m_bufferMemory, nullptr);
    }

    inline T& data() { return m_bufferData; }
    inline const T& data() const { return m_bufferData; }

    inline const VkBuffer& buffer() const { return m_buffer; }
    inline const VkDeviceMemory& memory() const { return m_bufferMemory; }

    /**
     * @brief Wrapper for create a new buffer
     * @param size Is the size of the buffer you want
     * @param usage Is a flag to describe for what usage the buffer is destinate
     * @param properties Is for find the memory type
     */
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
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
          .memoryTypeIndex = misc::findMemoryType(m_device.physical(), memRequirements.memoryTypeBits, properties),
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

    /**
     * @brief Just update the buffer, for example if in imgui with change the value of bufferData
     */
    void update(float time, uint32_t currentImage) {
      void* data;
      vkMapMemory(m_device.logical(), m_bufferMemory, 0, m_bufferSize, 0, &data);
      memcpy(data, &m_bufferData, m_bufferSize);
      vkUnmapMemory(m_device.logical(), m_bufferMemory);
    }

  protected:
    T m_bufferData;  // normaly is struct type like Depth, MVP, Vertex, Material ...

    VkBuffer m_buffer;
    VkDeviceMemory m_bufferMemory;
    VkDeviceSize m_bufferSize;

    const Device& m_device;
  };

  /*
   * Specification
   */

  class VertexBuffer : public Buffer<std::vector<Vertex>> {
  public:
    VertexBuffer(const Device& device, const std::vector<Vertex>& vertices) : Buffer(device, vertices) {
      m_bufferSize = sizeof(Vertex) * m_bufferData.size();

      createBuffer(m_bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

      void* data;
      vkMapMemory(m_device.logical(), m_bufferMemory, 0, m_bufferSize, 0, &data);
      memcpy(data, m_bufferData.data(), (size_t)m_bufferSize);
      vkUnmapMemory(m_device.logical(), m_bufferMemory);
    }
  };

  class MaterialBuffer : public Buffer<Material> {
  public:
    MaterialBuffer(const Device& device, const Material& material)
        : Buffer(device,
                 material,
                 VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {}
  };

}  // namespace vkl

#endif  // BUFFER_HPP