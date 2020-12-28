#ifndef RENDERPASS_HPP
#define RENDERPASS_HPP

#include <vulkan/vulkan.h>
#include <NonCopyable.hpp>
#include <vector>

namespace vkl {
  class Device;
  class SwapChain;

  /**
   * Note Exposé : Le render pass permet d'inquier combien chaque framebuffer
   * aura de buffers de couleur et de profondeur, combien de samples il faudra utiliser avec chaque
   * frambuffer et comment les utiliser tout au long des opérations de rendu.
   */

  class RenderPass : public NonCopyable {
  public:
    RenderPass(const Device& device, const SwapChain& swapChain);
    ~RenderPass();

    inline const VkRenderPass& handle() const { return m_renderPass; }
    inline const VkFramebuffer& frameBuffer(uint32_t index) const { return m_frameBuffers[index]; }
    inline size_t size() const { return m_frameBuffers.size(); }

    void recreate();
    void cleanupOld();

  protected:
    VkRenderPass m_renderPass;
    VkRenderPass m_oldRenderPass;

    std::vector<VkFramebuffer> m_frameBuffers;

    const Device& m_device;
    const SwapChain& m_swapChain;

    virtual void createRenderPass() = 0;
    void createFrameBuffers();

    void destroyFrameBuffers();
  };
}  // namespace vkl

#endif  // RENDERPASS_HPP