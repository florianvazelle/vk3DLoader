/**
 * @file RenderPass.hpp
 * @brief Define RenderPass base class
 */

#ifndef RENDERPASS_HPP
#define RENDERPASS_HPP

// clang-format off
#include <stddef.h>              // for size_t
#include <stdint.h>              // for uint32_t
#include <vulkan/vulkan_core.h>  // for VkFramebuffer, VkRenderPass, VkRende...
#include <NoCopy.hpp>       // for NoCopy
#include <iostream>              // for operator<<, cout, ostream
#include <memory>                // for unique_ptr
#include <vector>                // for vector
namespace vkl { class Device; } 
namespace vkl { class FrameBufferAttachment; }
namespace vkl { class SwapChain; }
// clang-format on

namespace vkl {
  class IRenderPass : public NoCopy {
  public:
    virtual ~IRenderPass() {}

    virtual const VkRenderPass& handle() const                        = 0;
    virtual const VkFramebuffer& frameBuffer(uint32_t index) const    = 0;
    virtual const FrameBufferAttachment& depthAttachment(int i) const = 0;
    virtual size_t size() const                                       = 0;

    virtual void recreate() = 0;

  protected:
    virtual void createRenderPass()    = 0;
    virtual void createFrameBuffers()  = 0;
    virtual void destroyFrameBuffers() = 0;
  };

  /**
   * Note Exposé : Le render pass permet d'indiquer combien chaque framebuffer
   * aura de buffers de couleur et de profondeur, combien de samples il faudra utiliser avec chaque
   * frambuffer et comment les utiliser tout au long des opérations de rendu.
   */
  class RenderPass : public IRenderPass {
  public:
    RenderPass(const Device& device, const SwapChain& swapChain);
    ~RenderPass();

    inline const VkRenderPass& handle() const { return m_renderPass; }
    inline const VkFramebuffer& frameBuffer(uint32_t index) const { return m_frameBuffers[index]; }
    inline const FrameBufferAttachment& depthAttachment(int i) const { return *(m_depthAttachments.at(i)); };
    inline size_t size() const { return m_frameBuffers.size(); }

    void recreate();

  protected:
    VkRenderPass m_renderPass;

    std::vector<VkFramebuffer> m_frameBuffers;
    std::vector<std::unique_ptr<FrameBufferAttachment>>
        m_depthAttachments;  // TODO : transfomer ca en vector d'Attachment ou Attachment
                             // serait composer de plusieur FrameBufferAttachment (color, depth ...)

    const Device& m_device;
    const SwapChain& m_swapChain;

    void createRenderPass() { std::cout << "Warning: call base createRenderPass" << std::endl; };
    void createFrameBuffers();
    void destroyFrameBuffers();
  };
}  // namespace vkl

#endif  // RENDERPASS_HPP