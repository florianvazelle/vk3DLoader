#ifndef DESCRIPTORSETLAYOUT_HPP
#define DESCRIPTORSETLAYOUT_HPP

#include <vulkan/vulkan.h>

#include <NonCopyable.hpp>
#include <vkl/Device.hpp>

namespace vkl {

  class DescriptorSetLayout : public NonCopyable {
  public:
    DescriptorSetLayout(const Device& device);
    ~DescriptorSetLayout();

    inline const VkDescriptorSetLayout& handle() const { return m_descriptorSetLayout; }

  private:
    VkDescriptorSetLayout m_descriptorSetLayout;

    const Device& m_device;
  };
}  // namespace vkl

#endif  // DESCRIPTORSETLAYOUT_HPP