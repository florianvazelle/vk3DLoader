#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <vulkan/vulkan.h>
#include <stdexcept>

#include <NonCopyable.hpp>
#include <vkl/Device.hpp>

namespace vkl {

  /**
   * @brief A base class for all buffer
   */
  class Buffer : public NonCopyable {
  public:
    Buffer(const Device& device);
    ~Buffer();

    inline const VkBuffer& buffer() const { return m_buffer; }
    inline const VkDeviceMemory& memory() const { return m_bufferMemory; }

    /**
     * @brief Wrapper for create a new buffer
     * @param size Is the size of the buffer you want
     * @param usage Is a flag to describe for what usage the buffer is destinate
     * @param properties Is for find the memory type
     */
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

  protected:
    VkBuffer m_buffer;
    VkDeviceMemory m_bufferMemory;

    const Device& m_device;
  };

}  // namespace vkl

#endif  // VERTEX_HPP