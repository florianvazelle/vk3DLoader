#ifndef DEPTHRENDERPASS_HPP
#define DEPTHRENDERPASS_HPP

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vkl/RenderPass.hpp>
#include <vkl/SwapChain.hpp>

namespace vkl {
  /**
   * Note Exposé : Ici, pas de recreate car ne tiens pas compte de la SwapChain
   */

  class DepthRenderPass : public RenderPass {
  public:
    DepthRenderPass(const Device& device, const SwapChain& swapChain);

    inline const VkSampler& sample() const { return m_depthSampler; }
    inline const VkImageView& view() const { return m_imageView; }

  private:
    VkSampler m_depthSampler;

    VkImage m_image;
    VkImageView m_imageView;
    VkDeviceMemory m_bufferMemory;

    void createRenderPass();
    void createFrameBuffers();
  };
}  // namespace vkl

#endif  // DEPTHRENDERPASS_HPP