#ifndef BASICRENDERPASS_HPP
#define BASICRENDERPASS_HPP

#include <vulkan/vulkan.h>
#include <common/FrameBufferAttachment.hpp>
#include <common/RenderPass.hpp>
#include <memory>
#include <stdexcept>

namespace vkl {
  class BasicRenderPass : public RenderPass {
  public:
    BasicRenderPass(const Device& device, const SwapChain& swapChain);

    inline const FrameBufferAttachment& depthAttachment(int i) const { return *(m_depthAttachments.at(i).get()); };

  private:
    std::vector<std::unique_ptr<FrameBufferAttachment>>
        m_depthAttachments;  // TODO : transfomer ca en vector d'Attachment ou Attachment
                             // serait composer de plusieur FrameBufferAttachment (color, depth ...)

    void createRenderPass();
    void createFrameBuffers();
  };
}  // namespace vkl

#endif  // BASICRENDERPASS_HPP