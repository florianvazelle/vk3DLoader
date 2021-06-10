#ifndef DEPTHRENDERPASS_HPP
#define DEPTHRENDERPASS_HPP

// clang-format off
#include <common/RenderPass.hpp>  // for RenderPass
namespace vkl { class Device; }
namespace vkl { class SwapChain; }
// clang-format on

namespace vkl {
  /**
   * Note Exposé : Ici, pas de recreate car ne tiens pas compte de la SwapChain
   * TODO : obsolete prendre en compte la swapchain
   */

  class DepthRenderPass : public RenderPass {
  public:
    DepthRenderPass(const Device& device, const SwapChain& swapChain);

  private:
    void createRenderPass();
    void createFrameBuffers();

    void destroyFrameBuffers();
  };
}  // namespace vkl

#endif  // DEPTHRENDERPASS_HPP