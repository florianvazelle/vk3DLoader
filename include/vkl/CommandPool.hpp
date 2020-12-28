#ifndef COMMANDPOOL_HPP
#define COMMANDPOOL_HPP

#include <vulkan/vulkan.h>
#include <NonCopyable.hpp>

namespace vkl {
  class Device;

  /**
   * Note Exposé : Les command pools gèrent la mémoire utilisée par les command buffers, et ce sont
   * les command pools qui instancient les command buffers.
   */

  class CommandPool : public NonCopyable {
  public:
    CommandPool(const Device& device, const VkCommandPoolCreateFlags& flags);
    ~CommandPool();

    inline const VkCommandPool& handle() const { return m_pool; };

  private:
    VkCommandPool m_pool;
    VkCommandPoolCreateFlags m_flags;

    const Device& m_device;
  };
}  // namespace vkl

#endif  // COMMANDPOOL_HPP