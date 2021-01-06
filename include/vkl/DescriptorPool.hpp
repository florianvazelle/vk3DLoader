#ifndef DESCRIPTORPOOL_HPP
#define DESCRIPTORPOOL_HPP

#include <vulkan/vulkan.h>
#include <NonCopyable.hpp>
#include <vkl/Device.hpp>
#include <vkl/SwapChain.hpp>

namespace vkl {

  class DescriptorPool : public NonCopyable {
  public:
    DescriptorPool(const Device& device, const VkDescriptorPoolCreateInfo& poolInfo);
    ~DescriptorPool();

    inline const VkDescriptorPool& handle() const { return m_pool; };

    void cleanup();
    void recreate();

  private:
    VkDescriptorPool m_pool;

    const Device& m_device;
    const VkDescriptorPoolCreateInfo& m_poolInfo;

    void createDescriptorPool();
  };
}  // namespace vkl

#endif  // DESCRIPTORPOOL_HPP