#ifndef BASICRENDERPASS_HPP
#define BASICRENDERPASS_HPP

#include <poike/poike.hpp>

using namespace poike;

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