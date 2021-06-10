#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP

// clang-format off
#include <vulkan/vulkan_core.h>  // for VkSemaphore, VkSemaphore_T
#include <NonCopyable.hpp>       // for NonCopyable
namespace vkl { class Device; }
// clang-format on

namespace vkl {

  class Semaphore : public NonCopyable {
  public:
    Semaphore(const Device& device);
    Semaphore() = delete;
    ~Semaphore();

    inline const VkSemaphore& handle() const { return m_semaphore; }

  private:
    VkSemaphore m_semaphore;

    const Device& m_device;
  };

}  // namespace vkl

#endif  // SEMAPHORE_HPP