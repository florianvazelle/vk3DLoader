#ifndef BASICRENDERPASS_HPP
#define BASICRENDERPASS_HPP

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vkl/RenderPass.hpp>

namespace vkl {
  class BasicRenderPass : public RenderPass {
  public:
    BasicRenderPass(const Device& device, const SwapChain& swapChain);

  private:
    void createRenderPass();
  };
}  // namespace vkl

#endif  // BASICRENDERPASS_HPP