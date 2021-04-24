#ifndef DEPTHRENDERPASS_HPP
#define DEPTHRENDERPASS_HPP

#include <vulkan/vulkan.h>
#include <common/FrameBufferAttachment.hpp>
#include <common/RenderPass.hpp>
#include <common/SwapChain.hpp>
#include <memory>
#include <stdexcept>

namespace vkl {
  /**
   * Note Exposé : Ici, pas de recreate car ne tiens pas compte de la SwapChain
   * TODO : obsolete prendre en compte la swapchain
   */

  class DepthRenderPass : public RenderPass {
  public:
    DepthRenderPass(const Device& device, const SwapChain& swapChain);

    inline const FrameBufferAttachment& depthAttachment(int i) const { return *(m_depthAttachments.at(i).get()); };

  private:
    std::vector<std::unique_ptr<FrameBufferAttachment>>
        m_depthAttachments;  // TODO : transfomer ca en vector d'Attachment ou Attachment
                             // serait composer de plusieur FrameBufferAttachment (color, depth ...)

    void createRenderPass();
    void createFrameBuffers();

    void destroyFrameBuffers();
  };
}  // namespace vkl

#endif  // DEPTHRENDERPASS_HPP