#ifndef DEPTHRENDERPASS_HPP
#define DEPTHRENDERPASS_HPP

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vkl/FrameBufferAttachment.hpp>
#include <vkl/RenderPass.hpp>
#include <vkl/SwapChain.hpp>

namespace vkl {
  /**
   * Note Exposé : Ici, pas de recreate car ne tiens pas compte de la SwapChain
   * TODO : obsolete prendre en compte la swapchain
   */

  class DepthRenderPass : public RenderPass {
  public:
    DepthRenderPass(const Device& device, const SwapChain& swapChain);

    inline const VkImageView& view() const { return m_depthFrameBuffer.view(); }
    inline VkImageView& view() { return m_depthFrameBuffer.view(); }

    inline const VkSampler& sample() const { return m_depthFrameBuffer.sample(); }
    inline VkSampler& sample() { return m_depthFrameBuffer.sample(); }

  private:
    FrameBufferAttachment m_depthFrameBuffer;

    void createRenderPass();
    void createFrameBuffers();
  };
}  // namespace vkl

#endif  // DEPTHRENDERPASS_HPP