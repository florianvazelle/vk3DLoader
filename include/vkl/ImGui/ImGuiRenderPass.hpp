#ifndef IMGUIRENDERPASS_HPP
#define IMGUIRENDERPASS_HPP

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vkl/RenderPass.hpp>

namespace vkl {
  class ImGuiRenderPass : public RenderPass {
  public:
    ImGuiRenderPass(const Device& device, const SwapChain& swapChain);

  private:
    void createRenderPass();
  };
}  // namespace vkl

#endif  // IMGUIRENDERPASS_HPP