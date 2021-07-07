#ifndef DEPTHRENDERPASS_HPP
#define DEPTHRENDERPASS_HPP

#include <poike/poike.hpp>

using namespace poike;

namespace vkl {
  /**
   * Note Exposé : Ici, pas de recreate car ne tiens pas compte de la SwapChain
   * TODO : obsolete prendre en compte la swapchain
   */

  class DepthRenderPass : public RenderPass {
  public:
    DepthRenderPass(const Device& device, const SwapChain& swapChain);

  private:
    void createRenderPass() final;
    void createFrameBuffers() final;
  };
}  // namespace vkl

#endif  // DEPTHRENDERPASS_HPP