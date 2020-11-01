#ifndef COMMANDPOOL_HPP
#define COMMANDPOOL_HPP

#include <vulkan/vulkan.h>
#include <NonCopyable.hpp>

namespace vkl {
  class Device;

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