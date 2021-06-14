#ifndef BASICRENDERPASS_HPP
#define BASICRENDERPASS_HPP

// clang-format off
#include <common/RenderPass.hpp>  // for RenderPass
namespace vkl { class Device; }
namespace vkl { class SwapChain; }
// clang-format on

namespace vkl {
  class BasicRenderPass : public RenderPass {
  public:
    BasicRenderPass(const Device& device, const SwapChain& swapChain);

  private:
    void createRenderPass() final;
    void createFrameBuffers() final;
  };
}  // namespace vkl

#endif  // BASICRENDERPASS_HPP