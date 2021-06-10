/**
 * @file FrameBufferAttachment.hpp
 * @brief Define FrameBufferAttachment class
 */

#ifndef FRAMEBUFFERATTACHMENT_HPP
#define FRAMEBUFFERATTACHMENT_HPP

// clang-format off
#include <vulkan/vulkan_core.h>  // for VkImage, VkImageView, VkSampler, VkF...
namespace vkl { class Device; }
namespace vkl { class SwapChain; }
// clang-format on

namespace vkl {

  class FrameBufferAttachment {
  public:
    FrameBufferAttachment(const Device& device, const SwapChain& swapChain, VkFormat format, VkImageUsageFlags usage);
    ~FrameBufferAttachment();

    inline const VkImage& image() const { return m_image; }
    inline VkImage& image() { return m_image; }

    inline const VkImageView& view() const { return m_imageView; }
    inline VkImageView& view() { return m_imageView; }

    inline const VkSampler& sample() const { return m_sampler; }
    inline VkSampler& sample() { return m_sampler; }

    void createSample();

  private:
    VkFormat m_format;

    VkImage m_image;
    VkDeviceMemory m_bufferMemory;
    VkImageView m_imageView;

    VkSampler m_sampler;

    const Device& m_device;
    const SwapChain& m_swapChain;
  };
}  // namespace vkl

#endif  // FRAMEBUFFERATTACHMENT_HPP