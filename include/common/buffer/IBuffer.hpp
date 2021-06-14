#ifndef IBUFFER_HPP
#define IBUFFER_HPP

#include <vulkan/vulkan.h>

#include <common/NoCopy.hpp>

namespace vkl {

  class IBuffer : public NoCopy {
  public:
    virtual ~IBuffer() = default;

    virtual const VkBuffer& buffer() const                   = 0;
    virtual const VkDeviceMemory& memory() const             = 0;
    virtual const VkDeviceSize& size() const                 = 0;
    virtual const VkDescriptorBufferInfo& descriptor() const = 0;
  };

  class IUniformBuffers : public NoCopy {
  public:
    virtual ~IUniformBuffers() = default;

    virtual const VkBuffer& buffer(int index) const                   = 0;
    virtual const VkDescriptorBufferInfo& descriptor(int index) const = 0;
  };

}  // namespace vkl

#endif  // IBUFFER_HPP